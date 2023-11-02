#include "GameScene.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <memory>
#include <list>

#include "MyMath.h"

//void GameScene::CheckAllCollisions() {
//	//判定対象AとBの座標
//	Vector3 posA, posB;
//#pragma region 自キャラと敵弾の当たり判定
//	//自キャラの座標
//	posA = player_->GetWorldPosition();
//
//	//自キャラと敵弾全ての当たり判定
//	for (EnemyBullet* enemyBullet : enemyBullets_) {
//		//敵弾の座標
//		posB = enemyBullet->GetWorldPosition();
//
//		if (!isPlayerDamage) {
//			//座標AとBの距離を求める
//			//球と球の交差判定
//			if (posA.z + 5.0f >= posB.z && posA.z <= posB.z + 5.0f) {
//				if (posA.y + 5.0f >= posB.y && posA.y <= posB.y + 5.0f) {
//					if (posA.x + 5.0f >= posB.x && posA.x <= posB.x + 5.0f) {
//						//自キャラの衝突時コールバックを呼び出す
//						player_->OnCollision();
//						//敵弾の衝突時コールバックを呼び出す
//						enemyBullet->OnCollision();
//					}
//				}
//			}
//		}
//	}
//#pragma endregion
//
//#pragma region 自弾と敵キャラの当たり判定
//	//自弾の座標
//	for (PlayerBullet* playerBullet : playerBullets_) {
//		posA = playerBullet->GetWorldPosition();
//
//		//自弾と敵キャラ全ての当たり判定
//		//敵キャラの座標
//		posB = enemy_->GetWorldPosition();
//
//		//座標AとBの距離を求める
//		//球と球の交差判定
//		if (posA.z + 5.0f >= posB.z && posA.z <= posB.z + 5.0f) {
//			if (posA.y + 5.0f >= posB.y && posA.y <= posB.y + 5.0f) {
//				if (posA.x + 5.0f >= posB.x && posA.x <= posB.x + 5.0f) {
//					//自弾の衝突時コールバックを呼び出す
//					playerBullet->OnCollision();
//					//敵キャラの衝突時コールバックを呼び出す
//					enemy_->OnCollision();
//					if (enemy_->IsDead()) {
//					}
//				}
//			}
//		}
//	}
//#pragma endregion
//}


//void GameScene::AllReset()
//{
//	player_->Reset();
//
//	enemy_->Reset();
//
//	for (PlayerBullet* playerBullet : playerBullets_) {
//		playerBullet->Reset();
//	}
//
//	for (EnemyBullet* enemyBullet : enemyBullets_) {
//		enemyBullet->Reset();
//	}
//
//	isPlayerDamage = false;
//}

GameScene::GameScene() {}

GameScene::~GameScene() {}

//void GameScene::TitleSceneInitialize() {
//	pressButtonWaitCount = 0;
//}
//void GameScene::TitleSceneUpdate() {
//	nowScene = TITLE;
//	if (++pressButtonWaitCount >= 20) {
//		if (button->isTrigger() || input_->TriggerKey(DIK_SPACE)) {
//			if (!transition_->GetIsChangeScene()) {
//				transition_->SetIsChangeScene(true);
//				transition_->Reset();
//				nextScene_ = GAME;
//				pressButtonWaitCount = 0;
//			}
//		}
//	}
//}
//void GameScene::TitleSceneDraw() {}
//void GameScene::TitleSceneDrawUI() {
//	backgroundSprites_[0]->Draw();
//	setumeiSprites_[0]->Draw();
//}

void GameScene::GameSceneInitialize() {
	//カメラの初期化
	camera_ = std::make_unique<RailCamera>();
	camera_->Initialize(worldTransform_, { 0.0f,0.0f,0.0f });

	//スカイドームの初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get());

	//グラウンドの初期化
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelGround_.get());

	//プレイヤーの初期化
	std::vector<uint32_t> playerTextures{playerTex_};
	std::vector<Model*> playerModels{playerModel_.get()};
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels, playerTextures);
	player_->SetGameScene(this);
	player_->SetViewProjection(&viewProjection_);

}
void GameScene::GameSceneUpdate() {
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

}
void GameScene::GameSceneDraw() {
	//スカイドームの描画
	//skydome_->Draw(viewProjection_);

	//グラウンドの更新
	//ground_->Draw(viewProjection_);

	//プレイヤーの描画
	player_->Draw(viewProjection_);
}
void GameScene::GameSceneDrawUI() {
}

//void GameScene::ClearSceneInitialize() {
//	pressButtonWaitCount = 0;
//}
//void GameScene::ClearSceneUpdate() {
//	nowScene = CLEAR;
//
//	if (++pressButtonWaitCount >= 20) {
//		if (button->isTrigger() || input_->TriggerKey(DIK_SPACE)) {
//			if (!transition_->GetIsChangeScene()) {
//				transition_->SetIsChangeScene(true);
//				transition_->Reset();
//				nextScene_ = TITLE;
//				pressButtonWaitCount = 0;
//			}
//		}
//	}
//}
//void GameScene::ClearSceneDraw() {}
//void GameScene::ClearSceneDrawUI() {
//	backgroundSprites_[1]->Draw();
//	setumeiSprites_[0]->Draw();
//}

//void GameScene::GameoverSceneInitialize() {
//	pressButtonWaitCount = 0;
//}
//void GameScene::GameoverSceneUpdate() {
//	nowScene = GAMEOVER;
//
//	if (++pressButtonWaitCount >= 20) {
//		if (button->isTrigger() || input_->TriggerKey(DIK_SPACE)) {
//			if (!transition_->GetIsChangeScene()) {
//				transition_->SetIsChangeScene(true);
//				transition_->Reset();
//				nextScene_ = TITLE;
//				pressButtonWaitCount = 0;
//			}
//		}
//	}
//}
//void GameScene::GameoverSceneDraw() {}
//void GameScene::GameoverSceneDrawUI() {
//	backgroundSprites_[2]->Draw();
//	setumeiSprites_[0]->Draw();
//}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// キー入力 初期化
	button->Initialize();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.farZ = 10000.0f;
	viewProjection_.Initialize();

	//テクスチャ
	playerTex_ = TextureManager::Load("blue.png");
	whiteTex_ = TextureManager::Load("white1x1.png");

	//スプライト

	//3Dモデル
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	modelGround_.reset(Model::CreateFromOBJ("ground", true));
	playerModel_.reset(Model::Create());

	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向が参照するビュープロジェクションを指定する(アドレスなし)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	GameSceneInitialize();
}

void GameScene::Update() {

	//キー入力の更新
	button->Update();
	GameSceneUpdate();

#ifdef _DEBUG

	ImGui::Begin("window");
	//ImGui::Text("scene %d", sceneCount_);
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

	GameSceneDraw();

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