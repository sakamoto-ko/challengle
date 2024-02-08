#pragma once
#include "Collider.h"

class LockOn;

class Weapon final : public Collider
{
private:
	std::vector<Model*> models_;
	WorldTransform worldTransform_;
	WorldTransform effectWorldTransform_;

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	int afterAttackStay_ = 20;
	bool isSwingUp_ = false;
	bool isSwingDown_ = false;
	bool isEnd_ = false;
	bool isStop_ = false;
	bool isReset_ = false;
	bool isEffect_ = false;

	float speed_ = 0.1f;
	int attackTime_ = 0;

public:
	void Initialize(const std::vector<Model*>& models);
	void Update(Vector3 translate);
	void Draw(const ViewProjection& viewProjection);
	const Vector3& GetRotation() const { return worldTransform_.rotation_; }
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }
	void SetParent(const WorldTransform* parent);
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }
	Vector3 GetCenterPosition() const override;
	//衝突コールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;
	//void OnCollision() override;

	bool GetAttackFinish() { return isEnd_; }
	bool GetIsReset() { return isReset_; }
	void SetIsReset(bool isReset) { isReset_ = isReset; }
	void SetScale(Vector3 scale) { effectWorldTransform_.scale_ = scale; }
	void Imgui();

	bool GetIsEffect() { return isEffect_; }
	void Effect();
	void EffectDraw(const ViewProjection& viewProjection);

	void Reset();
};

