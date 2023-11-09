#pragma once
#include "Skydome.h"
#include "Ground.h"
#include "RailCamera.h"
#include "player.h"
#include "Button.h"
#include "tempo.h"

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

	/*void TitleSceneInitialize();
	void TitleSceneUpdate();
	void TitleSceneDraw();
	void TitleSceneDrawUI();*/

	void GameSceneInitialize();
	void GameSceneUpdate();
	void GameSceneDraw();
	void GameSceneDrawUI();

	/*void ClearSceneInitialize();
	void ClearSceneUpdate();
	void ClearSceneDraw();
	void ClearSceneDrawUI();*/

	/*void GameoverSceneInitialize();
	void GameoverSceneUpdate();
	void GameoverSceneDraw();
	void GameoverSceneDrawUI();*/

	//衝突判定
	void CheckAllCollisions();

	void AllReset();

	//自弾の追加
	void AddPlayerBullet(PlayerBullet* playerBullet);

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	uint32_t whiteTex_ = 0u;

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

	//自弾
	std::list<PlayerBullet*> playerBullets_;
	std::unique_ptr<Model>  playerBulletModel_ = nullptr;
	uint32_t playerBulletTex_ = 0u;
	//弾リストを取得
	const std::list<PlayerBullet*>& GetPlayerBullets() { return playerBullets_; }

	//テンポ
	std::unique_ptr<tempo> tempo_ = nullptr;
};
