#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <cassert>

    class Skydome {
public:
	void Initialize(Model* model, const Vector3& position);

	void Update();

	void Draw(ViewProjection& view);

private:
	// ワールド返還データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;
};
