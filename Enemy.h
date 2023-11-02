#pragma once
#include "EnemyBullet.h"
#include "Button.h"
#include "scene.h"

#define ENEMY_BULLET_MAX 1

//GameSceneクラスの前方宣言
class GameScene;

class Enemy {
private:
	//ワールド変換データ
	//Base
	WorldTransform worldTransform_;
	//体
	WorldTransform worldTransformBody_;
	//頭
	WorldTransform worldTransformHead_;
	//左手
	WorldTransform worldTransformArmL_;
	//右手
	WorldTransform worldTransformArmR_;
	//左足
	WorldTransform worldTransformLegL_;
	//右足
	WorldTransform worldTransformLegR_;
	//武器
	WorldTransform worldTransformWeapon_;
	//左羽根
	WorldTransform worldTransformWingL1_;
	WorldTransform worldTransformWingL2_;
	WorldTransform worldTransformWingL3_;
	WorldTransform worldTransformWingL4_;
	//右羽根
	WorldTransform worldTransformWingR1_;
	WorldTransform worldTransformWingR2_;
	WorldTransform worldTransformWingR3_;
	WorldTransform worldTransformWingR4_;

	//モデル
	std::vector<Model*> models_;
	std::vector<Model*> bulletModels_;

	//テクスチャハンドル
	std::vector<uint32_t> textures_;
	uint32_t tex_ = 0u;
	std::vector<uint32_t> bulletTextures_;
	uint32_t bulletTex_ = 0u;

	//キーボード入力
	Input* input_ = nullptr;
	Button* button = Button::GetInstance();

	//ゲームシーン
	GameScene* gameScene_ = nullptr;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	//フラグ
	bool isDead_ = false;
	bool isAttack = false;
	bool isAttack2 = false;
	bool isHit = false;

	//HP
	int32_t hp = 100;
	int32_t hp_start = 100;

	//速度
	Vector3 velocity_ = {};

	Vector3 moveMax = { 18.0f ,5.0f,1.0f };
	Vector3 moveMin = { -3.0f,-4.6f, 1.0f };

	//カウント
	int moveTime = 0;
	int hitTime = 0;
	int attackWaitCount = 0;
	int attack2WaitCount = 0;
	int attackCount = 0;

	enum class Behavior {
		kRoot,//通常
		kHit,//被弾
		kHalfHp,//体力半分
		kAttack1,//攻撃1
		kAttack2,//攻撃2
	};
	Behavior behavior_ = Behavior::kAttack1;

	//次のふるまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int enemyBulletNum = 0;

	bool isHalfHp = false;
	int halfHpCount = 0;

	Vector3 bulletVelocity[2] = {};
	int attackNum = 0;

	float speed = 0.5f;

	float floatingParameter_ = 0.0f;

public:
	Enemy();
	~Enemy();
	//初期化
	void Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures);
	//更新
	void Update(const ViewProjection viewProjection);
	//描画
	void Draw(const ViewProjection& viewProjection);
	void DrawUI();

	//弾発射
	void Shot();

	//セッター
	void SetParent(const WorldTransform* parent);
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetModels(const std::vector<Model*>& models) { models_ = models; }
	void SetTextures(const std::vector<uint32_t>& textures) { textures_ = textures; }

	//ゲッター
	Vector3 GetWorldPosition();
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	const ViewProjection* GetViewProjection() { return viewProjection_; }
	bool IsDead() { return isDead_; }
	int32_t GetEnemyHp() { return hp; }

	//当たり判定
	void OnCollision();

	//Behavior
	//通常行動初期化
	void BehaviorRootInitialize();
	//通常行動更新
	void BehaviorRootUpdate();

	//攻撃1行動初期化
	void BehaviorAttack1Initialize();
	//攻撃1行動更新
	void BehaviorAttack1Update();
	//攻撃2行動初期化
	void BehaviorAttack2Initialize();
	//攻撃2行動更新
	void BehaviorAttack2Update();
	//被弾時行動初期化
	void BehaviorHitInitialize();
	//被弾時行動更新
	void BehaviorHitUpdate();
	//体力半分時行動初期化
	void BehaviorHalfHpInitialize();
	//体力半分時行動更新
	void BehaviorHalfHpUpdate();

	//リセット
	void Reset();

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();

};
