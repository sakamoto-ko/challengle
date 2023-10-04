#include "PlayerBullet.h"

#include "MyMath.h"

#include <cassert>
#include <list>

Vector3 PlayerBullet::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos = {};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void PlayerBullet::OnCollision() {
	//デスフラグを立てる
	isDead_ = true;
}

PlayerBullet::PlayerBullet() {}
PlayerBullet::~PlayerBullet() {}

// 初期化
void PlayerBullet::Initialize(Model* model, uint32_t textureHandle, const Vector3& position, const Vector3 velocity) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	//引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
	velocity_.y = 0.0f;
}

// 更新
void PlayerBullet::Update() {
	//WorldTransformの更新
	worldTransform_.UpdateMatrix();

	//座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	//時間経過でデス
	if (--deathTimer_ < ~0) {
		isDead_ = true;
	}
}

// 描画
void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
