#pragma once
#include <sstream>
#include <memory>
#include "Skydome.h"
#include "Ground.h"
#include "RailCamera.h"

typedef enum {
	TITLE,
	GAME,
	GAMEOVER,
	CLEAR,
}Scene;

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

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

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_;

	//ビュープロジェクション
	ViewProjection viewProjection_;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr <Model> modelSkydome_ = nullptr;

	//グラウンド
	std::unique_ptr<Ground> ground_;
	std::unique_ptr <Model> modelGround_ = nullptr;

	//レールカメラ
	std::unique_ptr<RailCamera> railCamera_;
};
