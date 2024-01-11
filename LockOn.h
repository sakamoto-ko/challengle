#pragma once
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "XInput.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "MyMath.h"
#include <variant>
#include <map>
#include <fstream>
#include <iostream>
#include <memory>
#include "Gamepad.h"

//Enemyクラスの前方宣言
class Enemy;

class LockOn
{
public:
	LockOn();
	~LockOn();
	void Initialize();
	void Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);
	void Draw();

	void LockOnTargetSerch(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);

	bool OutOfRange(const ViewProjection& viewProjection);
	bool CheckDistanceCondition(Vector3 positionView);
	bool CheckAngleCondition(Vector3 positionView);
	bool CheckAllCondition(Vector3 positionView);

	bool IsLockOn();

	Vector3 GetLockOnPosition();
	Vector3 ConvertScreen(Vector3 pos, const ViewProjection& viewProjection);
	Vector3 ConvertView(Vector3 pos, const ViewProjection& viewProjection);

	bool TriggerL();

private:
	Input* input_ = nullptr;

	//テクスチャ
	uint32_t lockOnMarkTex_ = 0u;
	//スプライト
	std::unique_ptr<Sprite> sprite2DReticle_;

	//ロックオン対象
	Enemy* target_ = nullptr;

	//距離計算用変数
	//ラジアン度に変換
	float kDegreeToRadian = PAI / 180.0f;
	//最小距離
	float minDistance_ = 0.0f;
	//最大距離
	float maxDistance_ = 30.0f;
	//角度範囲
	float angleRenge_ = 90.0f * kDegreeToRadian;

	Gamepad::Button button[256];
};
