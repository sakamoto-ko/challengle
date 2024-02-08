#pragma once
#include <list>
#include "Collider.h"
#include "Model.h"
#include "WorldTransform.h"
#include "GlobalVariables.h"

class CollisionManager
{
private:
	//コライダー
	std::list<Collider*> colliders_;

	//デバッグ用モデル
	std::vector<Model*> models_;

	//可視化ON/OFF
	bool isCollider_ = true;
public:
	//初期化
	void Initialize(const std::vector<Model*>& models);

	//ワールドトランスフォームの更新
	void UpdateWorldTransform();

	//描画
	void Draw(const ViewProjection& viewProjection);

	//リセット
	void Reset();

	//衝突判定
	void CheckCollisitionPair(Collider* colliderA, Collider* colliderB);

	//全ての当たり判定チェック
	void CheckAllCollisions();

	void AddCollider(Collider* collider);

	//調整項目の適用
	void ApplyGlobalVariables();

};

