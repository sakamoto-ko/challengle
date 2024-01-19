#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"
#include "MyMath.h"
#include "GlobalVariables.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	//テクスチャ
	//textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデル
	model_.reset(Model::Create());
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	modelGround_.reset(Model::CreateFromOBJ("ground", true));
	modelFace_.reset(Model::CreateFromOBJ("face", true));
	modelBody_.reset(Model::CreateFromOBJ("body", true));
	modelL_arm_.reset(Model::CreateFromOBJ("left", true));
	modelR_arm_.reset(Model::CreateFromOBJ("right", true));
	modelWeapon_.reset(Model::CreateFromOBJ("hammer", true));
	modelEnemyBody_.reset(Model::CreateFromOBJ("enemy_body", true));
	modelEnemyL_arm_.reset(Model::CreateFromOBJ("enemy_weapon", true));
	modelEnemyR_arm_.reset(Model::CreateFromOBJ("enemy_weapon", true));

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.farZ = 10000.0f;
	viewProjection_.Initialize();

	//スカイドーム
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get());

	//グラウンド
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelGround_.get());

	//プレイヤー
	std::vector<Model*>playerModels = {
		modelFace_.get(),
		modelBody_.get(),
		modelL_arm_.get(),
		modelR_arm_.get(),
		modelWeapon_.get()
	};
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);

	//エネミー
	LoadEnemyPopData();
	UpdateEnemyPopCommands();

	//ロックオン
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	//デバッグカメラ
	debugCamera_ = new DebugCamera(WinApp::kWindowHeight, WinApp::kWindowWidth);

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

	//カメラのビュープロジェクションを自キャラにコピー
	player_->GetViewProjection(&followCamera_->GetViewProjection());
	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());
	//ロックオン座標を自キャラにわたす
	player_->SetLockOn(lockOn_.get());
	//ロックオン座標をカメラにわたす
	followCamera_->SetLockOn(lockOn_.get());

	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向が参照するビュープロジェクションを指定する(アドレスなし)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	//デバッグカメラの更新
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		if (isDebugCameraActive_ != 1) {
			isDebugCameraActive_ = true;
		}
		else {
			isDebugCameraActive_ = false;
		}
	}
#endif

	//ゲームパッドの状態を得る変数(XINPUT)
	//XINPUT_STATE joyState;

	//スカイドーム
	skydome_->Update();

	//グラウンド
	ground_->Update();

	//プレイヤー
	player_->Update();

	//敵の発生と更新
	UpdateEnemyPopCommands();
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Update();
	}

	//ロックオン
	lockOn_->Update(enemies_, viewProjection_);

	//カメラ
	if (isDebugCameraActive_) {
		//デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
	}
	else {
		//追従カメラ
		followCamera_->Update();
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	}
	//ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	skydome_->Draw(viewProjection_);
	ground_->Draw(viewProjection_);

	player_->Draw(viewProjection_);

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	lockOn_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

//敵発生データの読み込み
void GameScene::LoadEnemyPopData() {
	//ファイルを開く
	std::ifstream file;
	file.open("./Resources/enemyPop.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

//敵発生コマンドの更新
void GameScene::UpdateEnemyPopCommands() {
	//待機処理
	if (isWait) {
		waitTimer--;
		if (waitTimer <= 0) {
			//待機完了
			isWait = false;
		}
		return;
	}

	//1行文の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		//1行文分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');
		//"//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}
		//POPコマンド
		if (word.find("POP") == 0) {
			//X座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//Y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//Z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//敵を発生させる
			EnemyPop(Vector3(x, y, z));
		}
		//WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機開始
			isWait = true;
			waitTimer = waitTime;

			//コマンドループを抜ける
			break;//重要
		}
	}
}

//敵発生関数
void GameScene::EnemyPop(Vector3 pos) {
	//敵モデルの生成
	std::vector<Model*>enemyModels = {
		modelEnemyBody_.get(),
		modelEnemyL_arm_.get(),
		modelEnemyR_arm_.get()
	};

	//敵の生成
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	
	//初期化
	newEnemy->Initialize(enemyModels);
	//リストに敵を登録する, std::moveでユニークポインタの所有権移動
	enemies_.push_back(std::move(newEnemy));

	//イテレータ
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		//各セッターに値を代入
		SetEnemyPopPos(pos);
		enemy->GetViewProjection(&followCamera_->GetViewProjection());
		enemy->SetGameScene(this);
		//更新
		enemy->Update();
	}

	//イテレータ
	//for (auto enemy = enemies_.begin(); enemy != enemies_.end(); ++enemy) {
	//	//各セッターに値を入
	//	SetEnemyPopPos(pos);
	//	enemy->get()->SetViewPRojection(&followCamera_->GetViewProjection());
	//	enemy->get()->SetGameScene(this);

	//	//更新
	//	enemy->get()->Update();
	//}
}