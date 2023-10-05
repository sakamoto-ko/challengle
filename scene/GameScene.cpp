#include "GameScene.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <memory>
#include <list>

#include "MyMath.h"

void GameScene::CheckAllCollisions() {
	//判定対象AとBの座標
	Vector3 posA, posB;
	//#pragma region 自キャラと敵弾の当たり判定
	//	//自キャラの座標
	//	posA = player_->GetWorldPosition();
	//
	//	//自キャラと敵弾全ての当たり判定
	//	for (EnemyBullet* enemyBullet : enemyBullets_) {
	//		//敵弾の座標
	//		posB = enemyBullet->GetWorldTransform();
	//
	//		//座標AとBの距離を求める
	//		//球と球の交差判定
	//		if (posA.z + playerRadius >= posB.z && posA.z <= posB.z + enemyBulletRadius) {
	//			if (posA.y + playerRadius >= posB.y && posA.y <= posB.y + enemyBulletRadius) {
	//				if (posA.x + playerRadius >= posB.x && posA.x <= posB.x + enemyBulletRadius) {
	//					//自キャラの衝突時コールバックを呼び出す
	//					player_->OnCollision();
	//					//敵弾の衝突時コールバックを呼び出す
	//					enemyBullet->OnCollision();
	//				}
	//			}
	//		}
	//	}
	//#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	//自弾の座標
	for (PlayerBullet* playerBullet : playerBullets_) {
		posA = playerBullet->GetWorldPosition();

		//自弾と敵キャラ全ての当たり判定
		//敵キャラの座標
		posB = enemy_->GetWorldPosition();

		//座標AとBの距離を求める
		//球と球の交差判定
		if (posA.z + 1.0f >= posB.z && posA.z <= posB.z + 4.0f) {
			if (posA.y + 1.0f >= posB.y && posA.y <= posB.y + 4.0f) {
				if (posA.x + 1.0f >= posB.x && posA.x <= posB.x + 4.0f) {
					//自弾の衝突時コールバックを呼び出す
					playerBullet->OnCollision();
					//敵キャラの衝突時コールバックを呼び出す
					enemy_->OnCollision();
					if (enemy_->IsDead()) {
					}
				}
			}
		}
	}
#pragma endregion

	//#pragma region 自弾と敵弾の当たり判定
	//	//自弾の座標
	//	for (PlayerBullet* playerBullet : playerBullets_) {
	//		posA = playerBullet->GetWorldPosition();
	//
	//		//自弾と敵弾全ての当たり判定
	//		//敵弾の座標
	//		for (EnemyBullet* enemyBullet : enemyBullets_) {
	//			posB = enemyBullet->GetWorldTransform();
	//
	//			//座標AとBの距離を求める
	//			//球と球の交差判定
	//			if (posA.z + playerRadius >= posB.z && posA.z <= posB.z + enemyBulletRadius) {
	//				if (posA.y + playerRadius >= posB.y && posA.y <= posB.y + enemyBulletRadius) {
	//					if (posA.x + playerRadius >= posB.x && posA.x <= posB.x + enemyBulletRadius) {
	//						//自弾の衝突時コールバックを呼び出す
	//						playerBullet->OnCollision();
	//						//敵弾の衝突時コールバックを呼び出す
	//						enemyBullet->OnCollision();
	//					}
	//				}
	//			}
	//		}
	//	}
	//#pragma endregion
}

//自弾を追加する
void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	//リストに登録する
	playerBullets_.push_back(playerBullet);
}

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
	audio_ = Audio::GetInstance();

	// キー入力 初期化
	button->Initialize();

	//3Dモデルの生成
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	modelGround_.reset(Model::CreateFromOBJ("ground", true));

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.farZ = 10000.0f;
	viewProjection_.Initialize();

	//スカイドームの初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get());

	//グラウンドの初期化
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelGround_.get());

	//カメラの初期化
	camera_ = std::make_unique<RailCamera>();
	camera_->Initialize(worldTransform_, { 0.0f,0.0f,0.0f });

	//プレイヤーの初期化
	playerTex_ = TextureManager::Load("red.png");
	playerModel_.reset(Model::Create());

	std::vector<Model*> playerModels{
		playerModel_.get()};
	std::vector<uint32_t> playerTextures{playerTex_};

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels, playerTextures);
	player_->SetGameScene(this);
	player_->SetViewProjection(&viewProjection_);

	//エネミーの初期化
	enemyTex_ = TextureManager::Load("red.png");
	enemyModel_.reset(Model::Create());

	std::vector<Model*> enemyModels{
		enemyModel_.get()};
	std::vector<uint32_t> enemyTextures{enemyTex_};

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModels, enemyTextures);

	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向が参照するビュープロジェクションを指定する(アドレスなし)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {

	//XINPUT_STATE joyState;

	//キー入力の更新
	button->Update();

	//ビュープロジェクション行列の更新と転送
	camera_->Update();
	viewProjection_.matView = camera_->GetViewProjection().matView;
	viewProjection_.matProjection = camera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	//スカイドームの更新
	skydome_->Update();

	//グラウンドの更新
	ground_->Update();

	//プレイヤーの更新
	player_->Update(viewProjection_);

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

	//エネミーの更新
	enemy_->Update(viewProjection_);

	CheckAllCollisions();
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

	//スカイドームの描画
	skydome_->Draw(viewProjection_);

	//グラウンドの更新
	ground_->Draw(viewProjection_);

	//プレイヤーの描画
	player_->Draw(viewProjection_);

	//自弾描画
	for (PlayerBullet* playerBullet : playerBullets_) {
		playerBullet->Draw(viewProjection_);
	}

	//エネミーの描画
	enemy_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}