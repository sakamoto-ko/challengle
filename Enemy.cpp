#include "Enemy.h"

#include "MyMath.h"

#include <cassert>
#include <list>

#include "GameScene.h"

Enemy::Enemy() {

}
Enemy::~Enemy() {
}

Vector3 Enemy::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos = {};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::OnCollision() {
	hp -= 20;
}

//親となるトランスフォームをセット
void Enemy::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

// 初期化
void Enemy::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	SetModels(models);
	SetTextures(textures);

	tex_ = textures_[0];

	worldTransform_.Initialize();

	worldTransform_.translation_ = { 12.0f,-0.02f,40.0f };
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.scale_ = { 5.0f,5.0f,5.0f };

	worldTransform_.UpdateMatrix();

	hp = 100;
}

// 更新
void Enemy::Update(const ViewProjection viewProjection) {

	//死ぬ
	if (hp <= 0) {
		isDead_ = true;
	}

	worldTransform_.UpdateMatrix();

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("enemy")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}

// 描画
void Enemy::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransform_, viewProjection, tex_);
}

//UI描画
void Enemy::DrawUI() {
}