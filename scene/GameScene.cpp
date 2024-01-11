#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"
#include "MyMath.h"
#include "GlobalVariables.h"

void GameScene::CheckAllCollisions() {
	//判定対象AとBの座標
	Vector3 posA, posB;
#pragma region 自弾と敵キャラの当たり判定
	//自弾の座標
	for (PlayerBullet* playerBullet : playerBullets_) {
		posA = playerBullet->GetWorldPosition();

		//自弾と敵キャラ全ての当たり判定
		//敵キャラの座標
		for (std::unique_ptr<Enemy>& enemy : enemies_) {
			posB = enemy->GetWorldPosition();

			//座標AとBの距離を求める
			//球と球の交差判定
			if (posA.z + 1.0f >= posB.z && posA.z <= posB.z + 1.0f) {
				if (posA.y + 1.0f >= posB.y && posA.y <= posB.y + 1.0f) {
					if (posA.x + 1.0f >= posB.x && posA.x <= posB.x + 1.0f) {
						//自弾の衝突時コールバックを呼び出す
						playerBullet->OnCollision();
						//敵キャラの衝突時コールバックを呼び出す
						enemy->OnCollision();
						killEnemyCount_++;
						enemyCount--;
					}
				}
			}
		}
	}
#pragma endregion

#pragma region 自キャラと敵キャラの当たり判定
	//自キャラの座標
	posA = player_->GetWorldPosition();

	//敵キャラの座標
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		posB = enemy->GetWorldPosition();

		//座標AとBの距離を求める
		//球と球の交差判定
		if (posA.z + 1.0f >= posB.z && posA.z <= posB.z + 1.0f) {
			if (posA.y + 1.0f >= posB.y && posA.y <= posB.y + 1.0f) {
				if (posA.x + 1.0f >= posB.x && posA.x <= posB.x + 1.0f) {
					//自弾の衝突時コールバックを呼び出す
					player_->OnCollision();
					//敵キャラの衝突時コールバックを呼び出す
					enemy->OnCollision();
				}
			}
		}
	}
#pragma endregion
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	//bullet_の開放
	for (PlayerBullet* playerBullet : playerBullets_) {
		delete playerBullet;
	}
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
	modelEnemyBody_.reset(Model::CreateFromOBJ("enemy_body", true));
	modelEnemyL_arm_.reset(Model::CreateFromOBJ("enemy_weapon", true));
	modelEnemyR_arm_.reset(Model::CreateFromOBJ("enemy_weapon", true));

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
		modelWeapon_.get()
	};
	player_ = std::make_unique<Player>();
	player_->SetGameScene(this);
	player_->Initialize(playerModels);

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

	//カメラのビュープロジェクションを自キャラにコピー
	player_->GetViewProjection(&followCamera_->GetViewProjection());
	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());

	//エネミー
	enemyCount = 0;

	//ロックオン
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向が参照するビュープロジェクションを指定する(アドレスなし)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	isTutorial = true;
}

void GameScene::Update() {

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
		//デスフラグの立った敵を削除
		enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
			if (enemy->IsDead()) {
				delete enemy.release();
				return true;
			}
			return false;
			}
		);

		//ロックオン
		lockOn_->Update(enemies_, viewProjection_);

		if (lockOn_->IsLockOn()) {
			player_->SetLockOnPosition(lockOn_->GetLockOnPosition());
		}

		//追従カメラ
		followCamera_->Update();
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		//ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();

		CheckAllCollisions();

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

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("enemyCount")) {
		ImGui::Text("count : %d", enemyCount);
		ImGui::Text("count : %d", killEnemyCount_);
		ImGui::Text("clear : %d", isGameClear_);
		ImGui::Text("over : %d", isGameOver_);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
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
	/*isWait = true;
	waitTimer = 5;*/
}

//敵発生関数
void GameScene::EnemyPop(Vector3 pos) {
	//敵モデルの生成
	std::vector<Model*>enemyModels = {
		modelEnemyBody_.get(),
		modelEnemyL_arm_.get(),
		modelEnemyR_arm_.get()
	};

	enemyCount++;
	//敵の生成
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();

	newEnemy->SetGameScene(this);
	newEnemy->SetPlayer(player_.get());
	//初期化
	newEnemy->Initialize(enemyModels);

	//リストに敵を登録する, std::moveでユニークポインタの所有権移動
	enemies_.push_back(std::move(newEnemy));

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

void GameScene::Reset() {

	player_->Reset();

	enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		delete enemy.release();
		return true;
		});
	playerBullets_.remove_if([](PlayerBullet* bullet) {
		delete bullet;
		return true;
		});

	enemyCount = 0;
	killEnemyCount_ = 0;

	isGameClear_ = false;
	isGameOver_ = false;
	isTutorial = true;
}

bool GameScene::CheckAllEnemyIsDead() {
	if (killEnemyCount_ >= 10) {
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