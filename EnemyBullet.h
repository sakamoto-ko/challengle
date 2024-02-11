#pragma once
#include "Model.h"
#include "WorldTransform.h"

class EnemyBullet {

public:
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	Vector3 GetWorldPosition();

	// 衝突を検証したら呼び出される関数
	void OnCollision();

private:
	WorldTransform worldTransform_;
	Model* model_;
	uint32_t texturehandle_;
	Vector3 velocity_;
	// Vector3 Add(Vector3& a, Vector3& b);

	static const int32_t kLifeTime = 60;
	// デスタイマー
	int32_t deathtimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
};