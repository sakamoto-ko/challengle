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
	//enemy_の開放
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		delete enemy.release();
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
	modelHiEffect_.reset(Model::CreateFromOBJ("effect", true));
	modelEnemyBody_.reset(Model::CreateFromOBJ("enemy_body", true));
	modelEnemyL_arm_.reset(Model::CreateFromOBJ("enemy_weapon", true));
	modelEnemyR_arm_.reset(Model::CreateFromOBJ("enemy_weapon", true));
	modelCollider_.reset(Model::CreateFromOBJ("ico", true));

	// 画像
	tex_ = TextureManager::Load("UI/tutorial.png");
	sprites_.reset(Sprite::Create(tex_, { 0.0f, 0.0f }));
	sprites_->SetSize({ 852.0f, 480.0f });
	sprites_->SetTextureRect(
		{
			0.0f,
			0.0f,
		},
		{ 1280.0f, 720.0f });
	sprites_->SetPosition({ 228.0f, 120.0f });

	transition_ = TransitionEffect::GetInstance();

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
	};
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);

	std::vector<Model*>weaponModels = {
		modelWeapon_.get(),
		modelHiEffect_.get(),
	};
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(weaponModels);
	weapon_->SetParent(&player_->GetWorldTransform());

	//エネミー
	enemyCount = 5;

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
	//ロックオン座標を武器にわたす
	weapon_->SetLockOn(lockOn_.get());

	//衝突マネージャの生成
	std::vector<Model*>playerColliders = {
		modelCollider_.get(),
	};
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize(playerColliders);

	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向が参照するビュープロジェクションを指定する(アドレスなし)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	isTutorial = true;
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
	XINPUT_STATE joyState;

	//スカイドーム
	skydome_->Update();

	//グラウンド
	ground_->Update();

	if (isTutorial) {

		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			//Rトリガーを押していたら
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
				isTutorial = false;
			}
		}
	}
	else {
		//プレイヤー
		player_->Update();

		if (player_->GetAttack()) {
			if (!weapon_->GetIsReset()) {
				weapon_->Reset();
			}
			weapon_->Update(player_->GetCenterPosition());
			if (weapon_->GetAttackFinish()) {
				player_->SetBehaviorRoot();
				weapon_->SetIsReset(false);
			}
		}

		weapon_->Imgui();

		if (weapon_->GetIsEffect()) {
			weapon_->Effect();
		}

		//敵の発生と更新
		UpdateEnemyPopCommands();
		for (std::unique_ptr<Enemy>& enemy : enemies_) {
			enemy->Update();
			if (enemy->IsCheckDeaad()) {
				enemyCount--;
				enemy->SetReturnDead(false);
			}
		}
		//デスフラグの立った敵を削除
		/*enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
			if (enemy->IsDead()) {
				delete enemy.release();
				return true;
			}
			return false;
			}
		);*/

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

		//デバッグ表示用にワールドトランスフォームを更新
		collisionManager_->UpdateWorldTransform();

		//衝突判定と応答
		CheckAllColisions();
	}
	if (transition_->GetIsChangeScene()) {

		// ゲームシーンにフェードインする時、またはゲームシーンからフェードアウトする時更新

		if ((transition_->GetFadeIn() && transition_->GetNextScene() == CLEAR) ||
			(transition_->GetFadeIn() && transition_->GetNextScene() == GAMEOVER) ||
			(transition_->GetFadeIn() && transition_->GetNextScene() == RESET) ||
			(transition_->GetFadeOut() && transition_->GetNextScene() == GAME)) {
			transition_->Update();
		}
		// ゲームシーンからのフェードアウト終了でシーン遷移を止める
		else if (transition_->GetFadeIn() && transition_->GetNextScene() == GAME) {
			transition_->SetIsChangeScene(false);
			transition_->Reset();
		}
		// ゲームシーンへのフェードインが完了したら
		else {
			// 実際に遷移する
			transition_->ChangeScene();
		}
	}

	if (CheckAllEnemyIsDead()) {
		isGameClear_ = true;
	}
	else if (CheckPlayerIsDead()) {
		isGameOver_ = true;
	}
	// シーンチェンジ
	if (isGameClear_ || isGameOver_) {
		if (isGameClear_) { transition_->SetNextScene(CLEAR); }
		else if (isGameOver_) { transition_->SetNextScene(GAMEOVER); }
		transition_->SetIsChangeScene(true);
	}
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
	if (!isTutorial) {
		ground_->Draw(viewProjection_);

		for (std::unique_ptr<Enemy>& enemy : enemies_) {
			enemy->Draw(viewProjection_);
		}

		player_->Draw(viewProjection_);

		if (player_->GetAttack()) {
			weapon_->Draw(viewProjection_);
		}

		if (weapon_->GetIsEffect()) {
			weapon_->EffectDraw(viewProjection_);
		}
	}

#ifdef DEBUG

	collisionManager_->Draw(viewProjection_);

#endif // DEBUG

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

	if (isTutorial) {
		// タイトルの表示
		sprites_->Draw();
	}

	// 画面遷移の描画
	transition_->Draw();

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
}

void GameScene::CheckAllColisions()
{
	//weapon_->SetScale(Vector3{ 1.0f,1.0f,1.0f });

	//衝突マネージャのリセット
	collisionManager_->Reset();

	//コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());
	collisionManager_->AddCollider(weapon_.get());
	//敵全てについて
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		if (!enemy->IsDead()) {
			collisionManager_->AddCollider(enemy.get());
		}
	}

	//衝突判定と応答
	collisionManager_->CheckAllCollisions();
}

void GameScene::Reset() {

	std::vector<Model*>playerModels = {
		modelFace_.get(),
		modelBody_.get(),
		modelL_arm_.get(),
		modelR_arm_.get(),
	};
	player_->Reset(playerModels);

	enemyCount = 5;

	//エネミー
	/*enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
	if (enemy->IsDead()) {
		delete enemy.release();
		return true;
	}
	return false;
}
); */
	UpdateEnemyPopCommands();

	isGameClear_ = false;
	isGameOver_ = false;
	isTutorial = true;
}

bool GameScene::CheckAllEnemyIsDead() {
	if (enemyCount < 1) {
		for (std::unique_ptr<Enemy>& enemy : enemies_) {
			if (enemy->IsDead()) {
				return true;
			}
		}
	}

	return false;
}
bool GameScene::CheckPlayerIsDead() {
	if (player_->IsDead() == false) {
		return false;
	}

	return true;
}