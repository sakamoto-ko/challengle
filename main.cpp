#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "ClearScene.h"
#include "GameoverScene.h"
#include "TransitionEffect.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "WinApp.h"
#include "GlobalVariables.h"
#include "Scene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;
	TitleScene* titleScene = nullptr;
	GameScene* gameScene = nullptr;
	ClearScene* clearScene = nullptr;
	GameoverScene* gameoverScene = nullptr;
	// 画面遷移アニメーションの初期化
	TransitionEffect* transition_ = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"LE2C_09_サカモト_コウスケ_AL4");

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	uint32_t whiteTex_ = TextureManager::Load("UI/player.png");
	uint32_t blackTex_ = TextureManager::Load("UI/enemy.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	//グローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();

	transition_ = TransitionEffect::GetInstance();
	std::vector<uint32_t> transitionTextures{whiteTex_, blackTex_};
	transition_->Initialize(transitionTextures);

	// タイトルシーンの初期化
	titleScene = new TitleScene();
	titleScene->Initialize();

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();

	// クリアシーンの初期化
	clearScene = new ClearScene();
	clearScene->Initialize();

	// ゲームオーバーシーンの初期化
	gameoverScene = new GameoverScene();
	gameoverScene->Initialize();

	//ゲームシーンのリセットフラグ
	bool isReset = false;

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		//グローバル変数の更新
		GlobalVariables::GetInstance()->Update();

		// ゲームシーンの毎フレーム処理
		//gameScene->Update();

		// シーン処理
		switch (transition_->GetCurrentScene()) {
		case TITLE:

			if (isReset) {
				isReset = false;
			}

			// タイトルシーンの毎フレーム処理
			titleScene->Update();
			break;
		case GAME:

			if (isReset == false) {
				gameScene->Reset();
				isReset = true;
			}
			if (titleScene->GetMoveTitleName()) {
				titleScene->SetMoveTitleName(false);
			}

			// ゲームシーンの毎フレーム処理
			gameScene->Update();
			break;
		case CLEAR:

			if (isReset) {
				isReset = false;
			}

			// タイトルシーンの毎フレーム処理
			clearScene->Update();
			break;
		case GAMEOVER:
			if (isReset) {
				isReset = false;
			}

			// タイトルシーンの毎フレーム処理
			gameoverScene->Update();
			break;
		case RESET:

			isReset = false;
			transition_->SetNextScene(GAME);
			transition_->ChangeScene();

			break;
		}

		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		//gameScene->Draw();

		// シーン処理
		switch (transition_->GetCurrentScene()) {
		case GAME:
			// ゲームシーンの描画
			gameScene->Draw();
			break;
		case TITLE:
			// タイトルシーンの描画
			titleScene->Draw();
			break;
		case CLEAR:
			// ゲームシーンの描画
			clearScene->Draw();
			break;
		case GAMEOVER:
			// ゲームシーンの描画
			gameoverScene->Draw();
			break;
		case RESET:
			break;
		}

		// 画面遷移の描画
		if (titleScene->GetChangeGameScene()) {
			// transition_->Draw();
		}

		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	SafeDelete(titleScene);
	SafeDelete(gameScene);
	SafeDelete(clearScene);
	SafeDelete(gameoverScene);

	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}