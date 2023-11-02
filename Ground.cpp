#include "Ground.h"
#include "MyMath.h"
#include <cassert>
#include "ImGuiManager.h"

Ground::Ground() {}
Ground::~Ground() {}
void Ground::Initialize(Model* model) {
	assert(model);

	model_ = model;
	worldTransform_.Initialize();

	worldTransform_.translation_ = { 0.0f,-1.0f,50.0f };
	worldTransform_.rotation_ = { -1.5f,0.0f,-3.14f };
	worldTransform_.scale_ = { 0.1f,0.03f,0.03f };

	worldTransform_.UpdateMatrix();
}

void Ground::Update() {
#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("ground")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	worldTransform_.UpdateMatrix();
}

void Ground::Draw(ViewProjection viewProjection_) {
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection_);
}