#include "EnemyBullet.h"
#include "newMath.h"
#include <assert.h>

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {

	assert(model);
	model_ = model;
	// テクスチャ読み込み
	texturehandle_ = TextureManager::Load("white1x1.png");

	// ワールドトランスフォーム
	worldTransform_.Initialize();
	// 引数で受け取った初期座標
	worldTransform_.translation_ = position;
	// 引数で受け取った速度をメンバに代入
	velocity_ = velocity;
}

Vector3 EnemyBullet::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

void EnemyBullet::OnCollision() { isDead_ = true; }

void EnemyBullet::Update() {

	worldTransform_.UpdateMatrix();
	// 座標を移動
	worldTransform_.translation_ = Math::Add(worldTransform_.translation_, velocity_);

	// 時間経過でデス
	if (--deathtimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& view) {
	model_->Draw(worldTransform_, view, texturehandle_);
}