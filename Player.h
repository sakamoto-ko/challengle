#pragma once
#include "BaseCharactor.h"
#include <optional>

class Player : public BaseCharacter{
private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformFace_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformWeapon_;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	//浮遊ギミックの媒介変数
	float idelArmAngleMax_ = 0.0f;

	uint16_t frame_ = 0;
	uint16_t floatingCycle_ = 0;
	float floatingAmplitude_ = 0;

	uint32_t kModelFace = 0;
	uint32_t kModelBody = 1;
	uint32_t kModelL_arm = 2;
	uint32_t kModelR_arm = 3;
	uint32_t kModelWeapon = 4;

	enum class Behavior {
		kRoot,//通常状態
		kAttack,//攻撃中
	};
	Behavior behavior_ = Behavior::kRoot;

	//次のふるまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int isAttack = false;
	int afterAttackStay = 20;

public:
	Player();
	~Player();
	void Initialize(const std::vector<Model*>& models) override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	const WorldTransform& GetWorldTransform() { return worldTransformBase_; }

	void SetViewPRojection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

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

	//調整項目の適用
	void ApplyGlobalVariables();
};