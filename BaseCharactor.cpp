#include "BaseCharactor.h"

void BaseCharacter::Initialize(const std::vector<Model*>& models) {
	models_ = models;
	worldTransform_.Initialize();
	Collider::Initialize();
}
void BaseCharacter::Update() {
	worldTransform_.UpdateMatrix();
}
void BaseCharacter::Draw(const ViewProjection& viewProjection) {
	//モデル描画
	for (Model* model : models_) {
		model->Draw(worldTransform_, viewProjection);
	}
}

Vector3 BaseCharacter::GetCenterPosition() const
{
	Vector3 worldPos = {
		.x = worldTransform_.matWorld_.m[3][0],
		.y = worldTransform_.matWorld_.m[3][1],
		.z = worldTransform_.matWorld_.m[3][2], };

	return worldPos;
}
