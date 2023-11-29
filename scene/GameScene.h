#pragma once
#include "Skydome.h"
#include "Ground.h"
#include "RailCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Button.h"
#include "tempo.h"
#include "LockOn.h"
#include "DropItem.h"

#define BLOCK_SIZE 48
//map
typedef enum{
	NONE,//0
	BLOCK,//1
	UPONLY,//2
	GOAL,//3
}MapInfo;

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

	//敵発生データの読み込み
	void LoadEnemyPopData();

	//敵発生コマンドの更新
	void UpdateEnemyPopCommands();

	//敵発生関数
	void EnemyPop(Vector3 pos);

	Vector3 GetEnemyPopPos() { return enemyPopPos; }
	void SetEnemyPopPos(Vector3 pos) { enemyPopPos = pos; }

	//アイテム発生データの読み込み
	void LoadItemPopData();

	//アイテム発生コマンドの更新
	void UpdateItemPopCommands();

	//アイテム発生
	void ItemPop(Vector3 pos);

	void SetItemPopPos(Vector3 pos) { itemPopPos = pos; }

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

	// エネミー
	std::list<std::unique_ptr<Enemy>> enemies_;
	std::stringstream enemyPopCommands;
	Vector3 enemyPopPos = {};
	//std::unique_ptr<Enemy> enemy_ = nullptr;
	std::unique_ptr<Model> enemyModel_ = nullptr;
	uint32_t enemyTex_ = 0u;

	//待機中フラグ
	bool isWait = false;
	//待機タイマー
	uint32_t waitTimer = 0;

	//待機中フラグ
	bool isItemWait = false;
	//待機タイマー
	uint32_t waitItemTimer = 0;

	//テンポ
	std::unique_ptr<tempo> tempo_ = nullptr;

	//ロックオン
	std::unique_ptr<LockOn> lockOn_;

	//アイテム
	std::list < std::unique_ptr<DropItem>> items_;
	std::stringstream itemPopCommands;
	Vector3 itemPopPos = {};
	std::unique_ptr<Model> itemModel_ = nullptr;
	uint32_t itemTex_ = 0u;

	//マップ
	int map[25][80] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
			0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	};
	int mapPlayerRightTopX;
	int mapPlayerRightTopY;
	int mapPlayerRightBottomX;
	int mapPlayerRightBottomY;
	int mapPlayerLeftTopX;
	int mapPlayerLeftTopY;
	int mapPlayerLeftBottomX;
	int mapPlayerLeftBottomY;

	// スプライトの最大数
	static const int32_t BLOCK_MAX = 10000;
	std::array<std::unique_ptr<Sprite>, BLOCK_MAX> mapPos1_ = {};
	std::array<std::unique_ptr<Sprite>, BLOCK_MAX> mapPos2_ = {};
	std::array<std::unique_ptr<Sprite>, BLOCK_MAX> mapPos3_ = {};
	uint32_t mapTextures_[3] = { 0u };
	int m = 0;
	int n = 0;
	int l = 0;
};
