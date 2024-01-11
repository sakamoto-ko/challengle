#pragma once
#include "BaseCharactor.h"
#include "PlayerBullet.h"
#include <optional>

//GameSceneクラスの前方宣言
class GameScene;

class Player : public BaseCharacter {
private:
	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	std::vector<uint32_t> textures_;
	uint32_t tex_ = 0u;

	uint32_t kModelFace = 0;
	uint32_t kModelBody = 1;
	uint32_t kModelL_arm = 2;
	uint32_t kModelR_arm = 3;

	WorldTransform worldTransformBase_;
	WorldTransform worldTransformFace_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	//速度
	Vector3 velocity_ = {};

	//浮遊ギミックの媒介変数
	float idelArmAngleMax_ = 0.0f;

	uint16_t frame_ = 0;
	uint32_t floatingCycle_ = 0;
	float floatingAmplitude_ = 0;

	enum class Behavior {
		kRoot,//通常状態
		kAttack,//攻撃中
		kJump,//ジャンプ中
	};
	Behavior behavior_ = Behavior::kRoot;

	//次のふるまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int isAttack = false;
	int isJump = false;
	int afterAttackStay = 20;

	//ゲームシーン
	GameScene* gameScene_ = nullptr;

	//レティクル用
	std::unique_ptr<Sprite> spritePos_ = {};
	WorldTransform worldTransformReticle_;

	int playerBulletNum_ = 0;
	int playerBulletMax_ = 1;

	std::vector<uint32_t> bulletTextures_;
	uint32_t bulletTex_ = 0u;
	std::vector<Model*> bulletModels_;

	bool isDead_ = false;
public:
	Player();
	~Player();
	void Initialize(const std::vector<Model*>& models) override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	const WorldTransform& GetWorldTransform() { return worldTransformBase_; }

	void GetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

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

	//ジャンプ行動初期化
	void BehaviorJumpInitialize();
	//ジャンプ行動更新
	void BehaviorJumpUpdate();

	//調整項目の適用
	void ApplyGlobalVariables();

	void Move(float y);

	Vector3 GetWorldPosition();

	void GetReticlePosition(const ViewProjection viewProjection);

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetLockOnPosition(Vector3 reticlePos) { worldTransformReticle_.translation_ = reticlePos; }
	bool IsDead() const { return isDead_; }

	//衝突を検出したら呼び出されるコールバック関数	
	void OnCollision();
	void Reset();
};