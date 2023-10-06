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

	//ファイル名を指定してテクスチャを読み込む
	//textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデルの生成
	model_.reset(Model::Create());

	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	modelGround_.reset(Model::CreateFromOBJ("ground", true));

	//自キャラモデル
	modelFace_.reset(Model::CreateFromOBJ("face", true));
	modelBody_.reset(Model::CreateFromOBJ("body", true));
	modelL_arm_.reset(Model::CreateFromOBJ("left", true));
	modelR_arm_.reset(Model::CreateFromOBJ("right", true));
	modelWeapon_.reset(Model::CreateFromOBJ("hammer", true));
	std::vector<Model*>playerModels = {
		modelFace_.get(),
		modelBody_.get(),
		modelL_arm_.get(),
		modelR_arm_.get(),
		modelWeapon_.get()
	};

	//敵キャラモデル
	modelEnemyBody_.reset(Model::CreateFromOBJ("enemy_body", true));
	modelEnemyL_arm_.reset(Model::CreateFromOBJ("enemy_weapon", true));
	modelEnemyR_arm_.reset(Model::CreateFromOBJ("enemy_weapon", true));
	std::vector<Model*>enemyModels = {
		modelEnemyBody_.get(),
		modelEnemyL_arm_.get(),
		modelEnemyR_arm_.get()
	};

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.farZ = 10000.0f;
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowHeight, WinApp::kWindowWidth);

	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向が参照するビュープロジェクションを指定する(アドレスなし)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get());

	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelGround_.get());

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(enemyModels);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

	//カメラのビュープロジェクションを自キャラにコピー
	player_->SetViewPRojection(&followCamera_->GetViewProjection());
	//カメラのビュープロジェクションを敵キャラにコピー
	enemy_->SetViewPRojection(&followCamera_->GetViewProjection());

	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());
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

	skydome_->Update();
	ground_->Update();

	player_->Update();

	enemy_->Update();

	if (isDebugCameraActive_) {
		//デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
	}
	else {
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