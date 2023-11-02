#include "EnemyBullet.h"

#include "MyMath.h"

#include <cassert>
#include <list>

Vector3 EnemyBullet::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos = {};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void EnemyBullet::OnCollision() {
	//デスフラグを立てる
	switch (attackNum) {
	case 1:
		break;
	default:
	case 2:
		isDead_ = true;
		break;
	}
}

EnemyBullet::EnemyBullet() {}
EnemyBullet::~EnemyBullet() {}

// 初期化
void EnemyBullet::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures, const Vector3& position, const Vector3 velocity) {

	SetModels(models);
	SetTextures(textures);
	tex_ = textures_[1];

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = { 0.0f,0.0f,0.1f };

	velocity_ = velocity;

	worldTransform_.UpdateMatrix();
}

// 更新
void EnemyBullet::Update() {
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	//攻撃1
	if (attackNum == 1) {
		worldTransform_.scale_ = { 3.0f,3.0f,3.0f };
		worldTransform_.rotation_.x += 0.314f;
	}
	//攻撃2
	else if (attackNum == 2) {
		worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
		if (worldTransform_.translation_.y >= 14.0f ||
			worldTransform_.translation_.y <= -14.0f) {
			velocity_.y *= -1;
		}
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("enemyBullet")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	//WorldTransformの更新
	worldTransform_.UpdateMatrix();

	//時間経過でデス
	if (--deathTimer_ < ~0) {
		isDead_ = true;
	}
	//画面外に出たらデス
	if (worldTransform_.translation_.x >= 50.0f) {
		isDead_ = true;
	}
}

// 描画
void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransform_, viewProjection, tex_);
}

void EnemyBullet::Reset()
{
	tex_ = textures_[1];
	isDead_ = true;

	worldTransform_.Initialize();

	worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.scale_ = { 0.0f,0.0f,0.0f };

	worldTransform_.UpdateMatrix();
}