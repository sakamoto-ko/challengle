#pragma once
#include "Audio.h"
#include "AxisIndicator.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
class FollowCamera
{
private:
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//追従対象
	const WorldTransform* target_ = nullptr;
public:
	FollowCamera();
	~FollowCamera();
	//初期化
	void Initialize();
	//更新
	void Update();
	const ViewProjection& GetViewProjection() { return viewProjection_; }
	const void SetTarget(const WorldTransform* target) { target_ = target; }
};

