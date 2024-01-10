#include "Enemy.h"
#include "MyMath.h"
#include <cassert>
#include <iostream>
#include "ImGuiManager.h"

#include "Player.h"
#include "GameScene.h"

//移動初期化
void Enemy::InitializeMoveGimmick() {
	assert(player_);
	Vector3 playerPos = player_->GetWorldPosition();

	worldTransformBase_.translation_.x = float(playerPos.x + (rand() % 20 - 20) + 10);
	worldTransformBase_.translation_.z = float(playerPos.z + (rand() % 20 - 20) + 10);

	isAppear_ = true;
}

//移動更新
void Enemy::UpdateMoveGimmick() {
	assert(player_);

	//移動(ベクトルを加算)
	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 worldPos = Subtract(playerPos, worldTransformBase_.translation_);
	Vector3 velocity = { worldPos };

	//速さ
	const float speed = 0.2f;
	//移動量に速さを反映
	velocity = Multiply(speed, Normalize(velocity));

	//移動ベクトルをカメラの角度だけ回転する
	Matrix4x4 rotate = MakeRotateMatrix(Multiply(1.0f, viewProjection_->rotation_));
	velocity = TransformNormal(velocity, rotate);

	worldTransformBase_.translation_.x += velocity.x;
	worldTransformBase_.translation_.z += velocity.z;

	//移動方向に向きを合わせる
	//Y軸周り角度(θy)
	worldTransformBase_.rotation_.y = std::atan2(-velocity.x, -velocity.z);

	//行動制限
	worldTransformBase_.translation_.x = Clamp(worldTransformBase_.translation_.x, -50.0f, 50.0f);
	worldTransformBase_.translation_.z = Clamp(worldTransformBase_.translation_.z, -55.0f, 55.0f);

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Enemy")) {
		ImGui::SliderFloat3("body.translation", &worldTransformBody_.translation_.x, -10.0f, 10.0f);
		ImGui::SliderFloat3("L_arm.translation", &worldTransformL_arm_.translation_.x, -10.0f, 10.0f);
		ImGui::SliderFloat3("R_arm.translation", &worldTransformR_arm_.translation_.x, -10.0f, 10.0f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}

Enemy::Enemy() {}
Enemy::~Enemy() {}

void Enemy::Initialize(const std::vector<Model*>& models) {
	//既定クラスの初期化
	BaseCharacter::Initialize(models);

	isAppear_ = false;

	//base
	worldTransformBase_.Initialize();
	worldTransformBase_.translation_.y = 2.0f;
	//体
	worldTransformBody_.Initialize();
	worldTransformBody_.parent_ = &worldTransformBase_;
	//左腕
	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.translation_.z -= 2.5f;
	//右腕
	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.translation_.z += 2.5f;

	InitializeMoveGimmick();

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}
void Enemy::Update() {
	//既定クラスの更新
	BaseCharacter::Update();

	assert(gameScene_);

	UpdateMoveGimmick();
}
void Enemy::Draw(const ViewProjection& viewProjection) {
	//既定クラスの描画
	//BaseCharacter::Draw(viewProjection);

	if (isAppear_) {
		models_[kModelBody]->Draw(worldTransformBody_, viewProjection);
		models_[kModelL_arm]->Draw(worldTransformL_arm_, viewProjection);
		models_[kModelR_arm]->Draw(worldTransformR_arm_, viewProjection);
	}
}

Vector3 Enemy::GetCenterPosition() {
	const Vector3 offset = { -1.1f,1.1f,0.0f };

	Vector3 result = TransformNormal(offset, worldTransformBase_.matWorld_);

	// TODO: return ステートメントをここに挿入します
	return result;
}

const Vector3 Enemy::constGetCenterPosition() {
	const Vector3 offset = { 0.0f,1.0f,0.0f };

	Vector3 result = TransformNormal(offset, worldTransform_.matWorld_);

	// TODO: return ステートメントをここに挿入します
	return result;
}
