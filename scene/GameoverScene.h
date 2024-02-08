#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Enemy.h"
#include "Ground.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "PrimitiveDrawer.h"
#include "SafeDelete.h"
#include "Skydome.h"
#include "Sprite.h"
#include "TransitionEffect.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <memory>

/// <summary>
/// タイトルシーン
/// </summary>
class GameoverScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameoverScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameoverScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// シーン遷移ゲッター
	bool GetChangeGameScene() { return isChangeGameScene_; }

	// シーン遷移セッター
	void SetChangeGameScene(bool flag) { isChangeGameScene_ = flag; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	PrimitiveDrawer* primitiveDrawer_ = nullptr;

	// 画面遷移アニメーション
	TransitionEffect* transition_ = nullptr;

	// 画像
	uint32_t tex_ = 0u;

	// スプライト
	std::unique_ptr<Sprite> sprite_;

	bool isChangeGameScene_ = false;
};