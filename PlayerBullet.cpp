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
void PlayerBullet::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures, const Vector3& position, const Vector3 velocity) {

	SetModels(models);
	SetTextures(textures);
	tex_ = textures_[0];

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	//引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
	velocity_.y = 0.0f;
	velocity_.z = 0.0f;
}

// 更新
void PlayerBullet::Update() {
	//座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("playerBullet")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
		ImGui::DragFloat3("velocity", &velocity_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	//WorldTransformの更新
	worldTransform_.UpdateMatrix();

	//時間経過でデス
	if (--deathTimer_ < ~0) {
		//isDead_ = true;
	}
	//画面外に出たらデス
	if (worldTransform_.translation_.x >= 50.0f) {
		isDead_ = true;
	}
}

// 描画
void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransform_, viewProjection, tex_);
}
