#pragma once
#include "ImGuiManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "newMath.h"

class RailCamera {

public:
	// 初期化
	void Initialize(const Vector3& position, const Vector3& rotate);

	// 更新
	void Updata();

	void SetPosition(Vector3 position) {
		worldTransform_.translation_.x = worldTransform_.translation_.x + position.x;
		worldTransform_.translation_.y = worldTransform_.translation_.y + position.y;
		worldTransform_.translation_.z = worldTransform_.translation_.z + position.z;
	}

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	const ViewProjection& GetViewProjection() { return viewProjection_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// ビュープロジェクション
	ViewProjection viewProjection_;
};
