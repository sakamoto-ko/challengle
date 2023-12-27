#include "Enemy.h"
#include "MyMath.h"
#include <cassert>
#include <iostream>
#include "ImGuiManager.h"

#include "Player.h"
#include "GameScene.h"

//移動初期化
void Enemy::InitializeMoveGimmick() {
	//中心座標
	center = { 0.0f,1.5f,12.0f };
	//角度
	angle = { 0.5f,0.5f,0.5f };
	//半径の長さ
	length = 10.0f;
}

//移動更新
void Enemy::UpdateMoveGimmick() {
	// 中心座標に角度と長さを使用した円の位置を加算する
	// 度数法の角度を弧度法に変換
	float radius = angle.x * 3.14f / 180.0f;
	// 三角関数を使用し、円の位置を割り出す。
	float add_x = cos(radius) * length;
	float add_z = sin(radius) * length;

	//速さ
	const float speed = 1.0f;
	//移動量
	Vector3 move = {
		add_x,
		0,
		add_z
	};
	//移動量に速さを反映
	move = Multiply(speed, Normalize(move));

	//移動ベクトルをカメラの角度だけ回転する
	Matrix4x4 rotate = MakeRotateMatrix(Multiply(1.0f, viewProjection_->rotation_));
	move = TransformNormal(move, rotate);

	//移動方向に向きを合わせる
	//Y軸周り角度(θy)
	worldTransformBase_.rotation_.y = std::atan2(move.x, move.z);

	//結果ででた位置を中心位置に加算し、それを描画位置とする
	worldTransformBase_.translation_.x = center.x + add_x;
	worldTransformBase_.translation_.z = center.z + add_z;

	// 角度更新
	angle.x += 1.0f;

#ifdef _DEBUG

	/*ImGui::Begin("window");
	if (ImGui::TreeNode("Enemy")) {
		ImGui::SliderFloat3("body.translation", &worldTransformBody_.translation_.x, -10.0f, 10.0f);
		ImGui::SliderFloat3("L_arm.translation", &worldTransformL_arm_.translation_.x, -10.0f, 10.0f);
		ImGui::SliderFloat3("R_arm.translation", &worldTransformR_arm_.translation_.x, -10.0f, 10.0f);
		ImGui::TreePop();
	}
	ImGui::End();*/

#endif // _DEBUG
}


Enemy::Enemy() {}
Enemy::~Enemy() {}

void Enemy::Initialize(const std::vector<Model*>& models) {
	//既定クラスの初期化
	BaseCharacter::Initialize(models);

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

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	UpdateMoveGimmick();
}
void Enemy::Draw(const ViewProjection& viewProjection) {
	//既定クラスの描画
	//BaseCharacter::Draw(viewProjection);

	models_[kModelBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelR_arm]->Draw(worldTransformR_arm_, viewProjection);
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
