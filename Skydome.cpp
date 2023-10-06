#include "Skydome.h"

#include "MyMath.h"

#include <cassert>
#include <list>

Skydome::Skydome() {}
Skydome::~Skydome() {}
void Skydome::Initialize(Model* model) {
	assert(model);

	model_ = model;
	worldTransform_.Initialize();
}

void Skydome::Update() {
	worldTransform_.scale_.x = 100;
	worldTransform_.scale_.y = 100;
	worldTransform_.scale_.z = 100;
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection viewProjection_) {
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection_);
}