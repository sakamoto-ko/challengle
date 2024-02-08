#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugCamera.h"
#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "Ground.h"
#include "FollowCamera.h"
#include "LockOn.h"
#include "CollisionManager.h"
#include "Weapon.h"
#include "TransitionEffect.h"

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

	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	//敵発生データの読み込み
	void LoadEnemyPopData();

	//敵発生コマンドの更新
	void UpdateEnemyPopCommands();

	//敵発生関数
	void EnemyPop(Vector3 pos);

	Vector3 GetEnemyPopPos() { return enemyPopPos; }
	void SetEnemyPopPos(Vector3 pos) { enemyPopPos = pos; }

	//衝突判定
	void CheckAllColisions();

	void Reset();

	bool GetIsReset() { return isReset_; }

	// シーン遷移ゲッター
	bool GetChangeGameScene() { return isChangeGameScene_; }

	// シーン遷移セッター
	void SetChangeGameScene(bool flag) { isChangeGameScene_ = flag; }

	//全ての敵が死んだかどうか確認するフラグ
	bool CheckAllEnemyIsDead();

	//プレイヤーが死んだかどうか確認するフラグ
	bool CheckPlayerIsDead();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	//テクスチャハンドル
	//uint32_t textureHandle_ = 0;

	// 画面遷移アニメーション
	TransitionEffect* transition_ = nullptr;

	//スプライト
	Sprite* sprite_ = nullptr;

	//3Dモデル
	std::unique_ptr <Model> model_ = nullptr;

	std::unique_ptr <Model> modelSkydome_ = nullptr;
	std::unique_ptr <Model> modelGround_ = nullptr;
	
	//プレイヤーモデル
	std::unique_ptr<Model> modelFace_;
	std::unique_ptr<Model> modelBody_;
	std::unique_ptr<Model> modelL_arm_;
	std::unique_ptr<Model> modelR_arm_;
	std::unique_ptr<Model> modelWeapon_;
	std::unique_ptr<Model> modelHiEffect_;
	//エネミーモデル
	std::unique_ptr<Model> modelEnemyBody_;
	std::unique_ptr<Model> modelEnemyL_arm_;
	std::unique_ptr<Model> modelEnemyR_arm_;
	//デバッグ用モデル
	std::unique_ptr<Model> modelCollider_;

	//ワールドトランスフォーム
	WorldTransform worldTransform_;

	//ビュープロジェクション
	ViewProjection viewProjection_;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	//プレイヤー
	std::unique_ptr<Player> player_;

	//エネミー
	//std::unique_ptr<Enemy> enemy_;
	std::list<std::unique_ptr<Enemy>> enemies_;
	//敵リストを取得
	//const std::list<std::unique_ptr<Enemy>>& GetEnemies() { return enemies_; }
	//敵発生コマンド
	std::stringstream enemyPopCommands;
	Vector3 enemyPopPos = {};

	//待機中フラグ
	bool isWait = false;
	//待機タイマー
	uint32_t waitTimer = 0;

	//スカイドーム
	std::unique_ptr<Skydome> skydome_;

	//グラウンド
	std::unique_ptr<Ground> ground_;

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	//ロックオン
	std::unique_ptr<LockOn> lockOn_;

	//衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;

	//衝突マネージャ
	std::unique_ptr<Weapon> weapon_;

	int enemyCount = 5;

	bool isReset_ = false;

	bool isChangeGameScene_ = false;

	bool isGameClear_ = false;

	bool isGameOver_ = false;

	// 画像
	uint32_t tex_ = 0u;

	// スプライト
	std::unique_ptr<Sprite> sprites_ = {};

	int killEnemyCount_ = 0;

	bool isTutorial = true;
};
