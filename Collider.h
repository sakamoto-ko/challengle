#pragma once
#include "Vector3.h"
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "CollisionTypeIdDef.h"

class Collider
{
private:
	//衝突半径
	float radius_ = 3.0f;

	//ワールド変換データ
	WorldTransform worldTransform_;

	uint32_t typeID_ = 0u;
public:
	//デストラクタ
	virtual ~Collider() = default;

	//初期化
	void Initialize();

	//ワールドトランスフォームの更新
	void UpdateWorldTransform();

	//描画
	void Draw(Model* model, const ViewProjection& viewProjection);

	//getter
	//半径
	float GetRadius() { return radius_; }
	//中心座用
	virtual Vector3 GetCenterPosition()const = 0;

	//setter
	//半径
	void SetRadius(float radius) { radius_ = radius; }

	virtual void SetScale(Vector3 scale) { worldTransform_.scale_ = scale; }

	//衝突コールバック関数
	virtual void OnCollision([[maybe_unused]] Collider* other);
	//virtual void OnCollision();

	//特別IDを取得
	uint32_t GetTypeID() const { return typeID_; }
	//特別IDを特定
	void SetTypeID(uint32_t typeID) { typeID_ = typeID; }
};

