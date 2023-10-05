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
}

void Ground::Update() {
	worldTransform_.translation_ = { 0.0f,0.0f,50.0f };
	worldTransform_.rotation_ = { -1.5f,0.0f,0.00f };
	worldTransform_.scale_ = { 0.1f,0.03f,0.03f };
	worldTransform_.UpdateMatrix();
}

void Ground::Draw(ViewProjection viewProjection_) {
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection_);
}