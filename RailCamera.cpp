#include "RailCamera.h"

void RailCamera::Initialize(const Vector3& position, const Vector3& rotate) {
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotate;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	viewProjection_.Initialize();
}

void RailCamera::Updata() {
	SetPosition({0.0f, 0.0f, float(0.0f)});

	ImGui::Begin("Camera");
	ImGui::SliderFloat3("trans", &worldTransform_.translation_.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("rotate", &worldTransform_.rotation_.x, -10.0f, 10.0f);
	ImGui::End();

	worldTransform_.matWorld_ = Math::MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	viewProjection_.matView = Math::Inverse(worldTransform_.matWorld_);

	viewProjection_.TransferMatrix();
}