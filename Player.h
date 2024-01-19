#pragma once
#include "BaseCharactor.h"
#include <optional>

class LockOn;

class Player : public BaseCharacter{
private:
	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	uint32_t kModelFace = 0;
	uint32_t kModelBody = 1;
	uint32_t kModelL_arm = 2;
	uint32_t kModelR_arm = 3;
	uint32_t kModelWeapon = 4;

	WorldTransform worldTransformBase_;
	WorldTransform worldTransformFace_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformWeapon_;

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

	int isAttack_ = false;
	int isJump_ = false;
	int afterAttackStay_ = 20;

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	bool isSwingUp_ = false;
	bool isSwingDown_ = false;

	float speed_ = 0.1f;
	int attackTime_ = 0;

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

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	void AllWorldTransformUpdateMatrix();

	void SetBehavior(Behavior behavior){ behaviorRequest_ = behavior; }

	void Reset(const std::vector<Model*>& models);
};