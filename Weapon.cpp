#include "Weapon.h"
#include "LockOn.h"
#include <cassert>
#include <iostream>
#include "ImGuiManager.h"
#include "MyMath.h"
#include "Enemy.h"

void Weapon::Initialize(const std::vector<Model*>& models)
{
	//既定クラスの初期化
	Collider::Initialize();

	models_ = models;

	worldTransform_.Initialize();
	effectWorldTransform_.Initialize();

	Reset();
	isReset_ = false;

	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
}

void Weapon::Update(Vector3 translate)
{
	//ロックオン中
	if (lockOn_ && lockOn_->ExistTarget()) {
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		Vector3 sub = Subtract(lockOnPosition, translate);

		//距離
		float distance = Length(sub);
		//距離閾値
		const float threshold = 0.2f;

		//閾値より離れている時のみ
		if (distance > threshold) {
			//閾値を超える速さなら補正する
			if (speed_ > distance - threshold) {
				//ロックオン対象へのめり込み防止
				speed_ = distance - threshold;
			}
		}
	}

	//振り上げて
	if (isSwingUp_) {
		worldTransform_.rotation_.x += speed_;
		if (worldTransform_.rotation_.x >= 0.6f) {
			isSwingUp_ = false;
			isSwingDown_ = true;
			attackTime_ = 0;
		}
	}
	//降り下ろす
	else if (isSwingDown_) {
		worldTransform_.rotation_.x -= speed_;
		if (++attackTime_ >= 20) {
			isSwingDown_ = false;
			attackTime_ = 0;
		}
	}
	//待機
	else if (!isSwingUp_ && !isSwingDown_) {
		if (--afterAttackStay_ <= 0) {
			attackTime_ = 0;
			isEnd_ = true;
			worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
		}
	}

	worldTransform_.UpdateMatrix();
}

void Weapon::Draw(const ViewProjection& viewProjection)
{
	models_[0]->Draw(worldTransform_, viewProjection);
}

Vector3 Weapon::GetCenterPosition() const
{
	const Vector3 offset = { 0.0f,20.0f,0.0f };

	Vector3 worldPos = TransformNormal(offset, worldTransform_.matWorld_);

	return worldPos;
}

void Weapon::OnCollision([[maybe_unused]] Collider* other)
//void Weapon::OnCollision()
{
	//衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		Enemy* enemy = reinterpret_cast<Enemy*>(other);
		//敵の位置にエフェクトを発生
		isEffect_ = true;
		effectWorldTransform_.scale_ = { 1.0f,1.0f,1.0f };
		effectWorldTransform_.translation_ = enemy->GetCenterPosition();
	}
}

void Weapon::Imgui()
{
	Vector3 centerPos = GetCenterPosition();

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Weapon")) {
		ImGui::SliderFloat3("weapon.translation_", &worldTransform_.translation_.x, -10, 10);
		ImGui::SliderFloat3("weapon.center", &centerPos.x, -10, 10);
		ImGui::SliderFloat3("effectWorldTransform_.scale", &effectWorldTransform_.scale_.x, -10, 10);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
	worldTransform_.UpdateMatrix();
	effectWorldTransform_.UpdateMatrix();

}

void Weapon::Reset()
{
	worldTransform_.translation_.y = -1.5f;

	effectWorldTransform_.translation_ = { 0.0f,0.0f,0.0f };
	effectWorldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	effectWorldTransform_.scale_ = { 1.0f,1.0f,1.0f };

	afterAttackStay_ = 20;
	speed_ = 0.1f;
	attackTime_ = 0;
	isSwingUp_ = true;
	isSwingDown_ = false;
	isEnd_ = false;
	isReset_ = true;
	isEffect_ = false;
}

void  Weapon::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

void Weapon::Effect() {
	effectWorldTransform_.scale_.x += 0.5f;
	effectWorldTransform_.scale_.y += 0.5f;
	effectWorldTransform_.scale_.z += 0.5f;
	if (effectWorldTransform_.scale_.z > 9.0f) {
		isEffect_ = false;
	}
	effectWorldTransform_.UpdateMatrix();
}

void Weapon::EffectDraw(const ViewProjection& viewProjection)
{
	models_[1]->Draw(effectWorldTransform_, viewProjection);
}
