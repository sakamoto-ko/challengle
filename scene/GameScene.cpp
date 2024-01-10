#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"
#include "MyMath.h"
#include "GlobalVariables.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	//bullet_の開放
	for (PlayerBullet* playerBullet : playerBullets_) {
		delete playerBullet;
	}
}

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

	//デバッグカメラ
	debugCamera_ = new DebugCamera(WinApp::kWindowHeight, WinApp::kWindowWidth);

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

	//カメラのビュープロジェクションを自キャラにコピー
	player_->GetViewProjection(&followCamera_->GetViewProjection());
	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//エネミー
	enemyCount = 0;
	UpdateEnemyPopCommands();

	//ロックオン
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

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

	//自弾更新
	for (PlayerBullet* playerBullet : playerBullets_) {
		playerBullet->Update();
	}
	//デスフラグの立った弾を削除
	playerBullets_.remove_if([](PlayerBullet* playerBullet) {
		if (playerBullet->IsDead()) {
			delete playerBullet;
			return true;
		}
		return false;
		}
	);

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

	//自弾描画
	for (PlayerBullet* playerBullet : playerBullets_) {
		playerBullet->Draw(viewProjection_);
	}

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

	if (enemyCount < 6) {
		//敵を発生させる
		EnemyPop(Vector3(0, 0, 0));
	}

	//待機開始
	isWait = true;
	waitTimer = 5;
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

	newEnemy->SetGameScene(this);
	newEnemy->SetPlayer(player_.get());
	//初期化
	newEnemy->Initialize(enemyModels);

	//リストに敵を登録する, std::moveでユニークポインタの所有権移動
	enemies_.push_back(std::move(newEnemy));
	enemyCount++;

	//イテレータ
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		//各セッターに値を代入
		SetEnemyPopPos(pos);
		enemy->GetViewProjection(&followCamera_->GetViewProjection());
		//更新
		enemy->Update();
	}
}

//自弾を追加する
void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	//リストに登録する
	playerBullets_.push_back(playerBullet);
}
