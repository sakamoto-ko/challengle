#pragma once
#include "Skydome.h"
#include "Ground.h"
#include "RailCamera.h"
#include "player.h"
#include "enemy.h"
#include "Button.h"

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

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	// キー入力
	Button* button = Button::GetInstance();

	//ビュープロジェクション
	ViewProjection viewProjection_;

	//レールカメラ
	std::unique_ptr<RailCamera> camera_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_ = nullptr;
	std::unique_ptr <Model> modelSkydome_ = nullptr;

	//グラウンド
	std::unique_ptr<Ground> ground_ = nullptr;
	std::unique_ptr <Model> modelGround_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<Model> playerModel_ = nullptr;
	uint32_t playerTex_ = 0u;

	//エネミー
	std::unique_ptr<Enemy> enemy_ = nullptr;
	std::unique_ptr<Model> enemyModel_ = nullptr;
	uint32_t enemyTex_ = 0u;
};
