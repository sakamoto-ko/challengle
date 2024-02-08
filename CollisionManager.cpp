#include "CollisionManager.h"
#include "MyMath.h"

void CollisionManager::Initialize(const std::vector<Model*>& models)
{
	GlobalVariables* grobalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Collider";

	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	grobalVariables->AddItem(groupName, "isCollider", isCollider_);

	ApplyGlobalVariables();

	models_ = models;
	isCollider_ = true;
}

void CollisionManager::UpdateWorldTransform()
{
	//非表示なら抜ける
	if (!isCollider_) {
		return;
	}

	for (Collider* collider : colliders_) {
		//更新
		collider->UpdateWorldTransform();
	}
}

void CollisionManager::Draw(const ViewProjection& viewProjection)
{
	//非表示なら抜ける
	if (!isCollider_) {
		return;
	}

	for (Collider* collider : colliders_) {
		//更新
		collider->Draw(models_[0], viewProjection);
	}
}

void CollisionManager::Reset()
{
	//リストを空っぽにする
	colliders_.clear();
}

void CollisionManager::CheckCollisitionPair(Collider* colliderA, Collider* colliderB)
{
	Vector3 posA = colliderA->GetCenterPosition();
	Vector3 posB = colliderB->GetCenterPosition();

	float distance = Length(Subtract(posB, posA));

	if (distance <= colliderA->GetRadius() + colliderB->GetRadius()) {
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);
		/*colliderA->OnCollision();
		colliderB->OnCollision();*/
	}
}

void CollisionManager::CheckAllCollisions()
{
	//リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		//イテレータBはイテレータAの次の要素から回す(重複判定を回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			//ペアの当たり判定
			CheckCollisitionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::AddCollider(Collider* collider) { colliders_.push_back(collider); }

void CollisionManager::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Collider";
	isCollider_ = globalVariables->GetBoolValue(groupName, "isCollider");
}
