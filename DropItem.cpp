#include "DropItem.h"

DropItem::DropItem() {}
DropItem::~DropItem() {}

void DropItem::Initialize(Vector3 pos, const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {
	SetModels(models);
	SetTextures(textures);

	Reset(pos);
}

void DropItem::Update() {
#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("item")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	worldTransform_.UpdateMatrix();
}

void DropItem::Draw(const ViewProjection& viewProjection) {
	if (!isDead_) {
		models_[0]->Draw(worldTransform_, viewProjection, tex_);
	}
}

void DropItem::OnCollision() {
	isDead_ = true;
}

Vector3 DropItem::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos = {};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void DropItem::Reset(Vector3 pos) {
	tex_ = textures_[0];

	//base
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.scale_ = { 0.2f,0.2f,0.2f };
	worldTransform_.UpdateMatrix();

	isDead_ = false;
}