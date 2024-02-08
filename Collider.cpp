#include "Collider.h"

void Collider::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 1.5f,1.5f,1.5f };
}

void Collider::UpdateWorldTransform()
{
	//ワールドトランスフォームの更新
	worldTransform_.translation_ = GetCenterPosition();
	worldTransform_.UpdateMatrix();
}

void Collider::Draw(Model* model, const ViewProjection& viewProjection)
{
	model->Draw(worldTransform_, viewProjection);
}

void Collider::OnCollision([[maybe_unused]] Collider* other)
//void Collider::OnCollision()
{
}
