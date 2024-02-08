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
#include <variant>
#include "Collider.h"

class BaseCharacter : public Collider
{
protected:
	//モデルデータ配列
	std::vector<Model*> models_;
	//ワールド変換データ
	WorldTransform worldTransform_;

public:
	//初期化
	//<param name = "models_">モデルデータ配列</param>
	virtual void Initialize(const std::vector<Model*>& models);

	//更新
	virtual void Update();

	//描画
	//<param name = "viewProjection">ビュープロジェクション(参照渡し)</param>
	virtual void Draw(const ViewProjection& viewProjection);

	//getter
	//ワールド変換データを取得
	//<returns">ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	//中心座標
	virtual Vector3 GetCenterPosition() const override;
};