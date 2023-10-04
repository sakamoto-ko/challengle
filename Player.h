#pragma once
#include "PlayerBullet.h"

//GameSceneクラスの前方宣言
class GameScene;

class Player {
private:
	//ワールド変換データ
	WorldTransform worldTransform_;

	//3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	//モデル
	Model* model_ = nullptr;
	Model* modelBullet_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	uint32_t textureHandleReticle_ = 0u;
	uint32_t textureHandleBullet_ = 0u;

	//2Dレティクル用スプライト
	Sprite* sprite2DReticle_ = nullptr;

	//キーボード入力
	Input* input_ = nullptr;

	//弾
	//std::list<PlayerBullet*> bullets_;
	
	int count_ = 0;

	//ゲームシーン
	GameScene* gameScene_ = nullptr;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	//浮遊ギミックの媒介変数
	float idelArmAngleMax_ = 0.0f;

	uint16_t frame_ = 0;
	int32_t floatingCycle_ = 0;
	float floatingAmplitude_ = 0;

	enum class Behavior {
		kRoot,//通常状態
		kAttack,//攻撃中
	};
	Behavior behavior_ = Behavior::kRoot;

	//次のふるまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int isAttack = false;
	int afterAttackStay = 20;

	//デスフラグ
	bool isDead_ = false;

	Vector3 move;

	int32_t hp = 100;

public:
	Player();
	~Player();
	//初期化
	void Initialize(Model* model, Model* modelBullet, uint32_t textureHandle, uint32_t textureHandleBullet, uint32_t textureHandleReticle);
	//更新
	void Update(const ViewProjection viewProjection);
	//描画
	void Draw(ViewProjection viewProjection);

	//旋回
	void Rotate();

	//攻撃
	void Attack();

	void Shot();

	//ワールド座標を取得
	Vector3 GetWorldPosition();

	void SetViewPRojection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	const ViewProjection* GetViewProjection() { return viewProjection_; }

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	//衝突を検出したら呼び出されるコールバック関数	
	void OnCollision();

	//浮遊ギミック初期化
	void InitializeFloatingGimmick();

	//浮遊ギミック更新
	void UpdateFloatingGimmick();

	//通常行動初期化
	void BehaviorRootInitialize();
	//通常行動更新
	void BehaviorRootUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//攻撃行動更新
	void BehaviorAttackUpdate();

	//親となるトランスフォームをセット
	void SetParent(const WorldTransform* parent);

	//UI描画
	void DrawUI();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	//自機のワールド座標から3Dレティクルのワールド座標を計算
	void Get3DReticleWorldPosition();

	//3Dレティクルもワールド座標から2Dレティクルのスクリーン座標を計算
	//void Convert3Dto2DCoord(const ViewProjection viewProjection);

	//マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	void GetMouseWorldPosition(const ViewProjection viewProjection);

	//
	bool IsDead() { return isDead_; }

	int32_t GetPlayerHp() { return hp; }
};
