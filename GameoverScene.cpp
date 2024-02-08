#include "GameoverScene.h"
#include "TextureManager.h"
#include <cassert>

GameoverScene::GameoverScene() {}
GameoverScene::~GameoverScene() {}

void GameoverScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	primitiveDrawer_ = PrimitiveDrawer::GetInstance();
	transition_ = TransitionEffect::GetInstance();

	// 画像
	tex_ = TextureManager::Load("UI/gameover.png");
	pressATex_ = TextureManager::Load("UI/pressA.png");

	sprite_.reset(Sprite::Create(tex_, { 0.0f, 0.0f }));
	sprite_->SetSize({ 1280.0f, 720.0f });
	sprite_->SetTextureRect(
		{
			0.0f,
			0.0f,
		},
		{ 1280.0f, 720.0f });
	sprite_->SetPosition({ 0.0f, 0.0f });

	pressA_.reset(Sprite::Create(pressATex_, { 0.0f, 0.0f }));
	pressA_->SetSize({ 1280.0f, 720.0f });
	pressA_->SetTextureRect(
		{
			0.0f,
			0.0f,
		},
		{ 1280.0f, 720.0f });
	pressA_->SetPosition({ 0.0f, 0.0f });
}

void GameoverScene::Update() {

	// シーンチェンジ/ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// シーンチェンジ中の処理
	if (transition_->GetIsChangeScene()) {

		// ゲームシーンにフェードインする時、またはゲームシーンからフェードアウトする時更新
		if ((transition_->GetFadeIn() && transition_->GetNextScene() == TITLE) ||
			(transition_->GetFadeOut() && transition_->GetNextScene() == GAMEOVER)) {
			transition_->Update();
		}
		// ゲームシーンからのフェードアウト終了でシーン遷移を止める
		else if (transition_->GetFadeIn() && transition_->GetNextScene() == GAMEOVER) {
			transition_->SetIsChangeScene(false);
			transition_->Reset();
		}
		// ゲームシーンへのフェードインが完了したら
		else {
			// 実際に遷移する
			transition_->ChangeScene();
		}
	}
	// シーンチェンジしていない時の処理
	else {
		// シーンチェンジ
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			//Rトリガーを押していたら
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
				transition_->SetIsChangeScene(true);
				// 遷移先のシーンをゲームにする
				transition_->SetNextScene(TITLE);
			}
		}
	}
}

void GameoverScene::Draw() {
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

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// タイトルの表示
	sprite_->Draw();
	pressA_->Draw();

	// 画面遷移の描画
	transition_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
