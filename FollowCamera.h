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

//前方宣言
class LockOn;

class FollowCamera
{
private:
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//追従対象
	const WorldTransform* target_ = nullptr;

	const LockOn* lockOn_ = nullptr;

public:
	FollowCamera();
	~FollowCamera();
	//初期化
	void Initialize();
	//更新
	void Update();
	const ViewProjection& GetViewProjection() { return viewProjection_; }
	const void SetTarget(const WorldTransform* target) { target_ = target; }
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }
};

