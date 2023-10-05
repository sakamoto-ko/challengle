#include "GameScene.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <memory>
#include <list>

#include "MyMath.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
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
	playerTex_ = TextureManager::Load("white1x1.png");
	playerModel_.reset(Model::Create());

	std::vector<Model*> playerModels{
		playerModel_.get()};
	std::vector<uint32_t> playerTextures{playerTex_};

	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels, playerTextures);

	//エネミーの初期化
	enemyTex_ = TextureManager::Load("white1x1.png");
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

	//スカイドームの更新
	skydome_->Update();

	//グラウンドの更新
	ground_->Update();

	//プレイヤーの更新
	player_->Update(viewProjection_);

	//エネミーの更新
	enemy_->Update(viewProjection_);

	//ビュープロジェクション行列の更新と転送
	camera_->Update();
	viewProjection_.matView = camera_->GetViewProjection().matView;
	viewProjection_.matProjection = camera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

#ifdef _DEBUG

	ImGui::Begin("window");
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

	//スカイドームの描画
	skydome_->Draw(viewProjection_);

	//グラウンドの更新
	ground_->Draw(viewProjection_);

	//プレイヤーの描画
	player_->Draw(viewProjection_);

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