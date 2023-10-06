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
	worldTransform_.translation_ = { 0,0,0 };
	worldTransform_.scale_.x = 0.1f;
	worldTransform_.scale_.z = 0.1f;
	worldTransform_.UpdateMatrix();
}

void Ground::Draw(ViewProjection viewProjection_) {
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection_);
}