#include "Enemy.h"

#include "MyMath.h"

#include <cassert>
#include <list>
#include <iostream>
#include <random>
#include <iomanip>

#include "GameScene.h"

Enemy::Enemy() {}
Enemy::~Enemy() {}

Vector3 Enemy::GetCenterPosition() {
	const Vector3 offset = { -1.1f,1.1f,0.0f };

	Vector3 result = TransformNormal(offset, worldTransform_.matWorld_);

	// TODO: return ステートメントをここに挿入します
	return result;
}

Vector3 Enemy::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos = {};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::OnCollision() {
	BehaviorSetHit();
}

//親となるトランスフォームをセット
void Enemy::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

// 初期化
void Enemy::Initialize(Vector3 pos, const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// キー入力 初期化
	button->Initialize();

	SetModels(models);
	SetTextures(textures);

	worldTransformTemp_.Initialize();
	worldTransformTemp_.translation_ = pos;
	worldTransformTemp_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformTemp_.scale_ = { 0.2f,0.2f,0.2f };
	worldTransformTemp_.UpdateMatrix();

	Reset();
}

// 更新
void Enemy::Update() {

	assert(gameScene_);

	//キー入力の更新
	button->Update();

	if (behaviorRequest_) {
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞い事の初期化を実行
		switch (behavior_) {
		case Enemy::Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;

		case Behavior::kHit:
			BehaviorHitInitialize();
			break;

		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;

		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}
		//振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
		//通常行動
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;

	case Behavior::kHit:
		BehaviorHitUpdate();
		break;

	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;

	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("enemy")) {
		if (ImGui::TreeNode("base")) {
			ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("behavior")) {
			ImGui::Text("behavior %d", behavior_);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	worldTransform_.UpdateMatrix();
}

// 描画
void Enemy::Draw(const ViewProjection& viewProjection) {
	if (!isDead_) {
		models_[0]->Draw(worldTransform_, viewProjection, tex_);
	}
}

//UI描画
void Enemy::DrawUI() {}

void Enemy::BehaviorSetRoot(){
	behaviorRequest_ = Behavior::kRoot;
}

//通常行動初期化
void Enemy::BehaviorRootInitialize() {
	velocity_ = { 0.0f,0.0f,0.0f };
}
//通常行動更新
void Enemy::BehaviorRootUpdate() {
	tex_ = textures_[1];

	//攻撃
	if (++AttackWaitTime >= 90) {
		BehaviorSetAttack();
	}

	//移動
	Move();
}

void Enemy::BehaviorSetAttack() {
	if (!isAttack) {
		velocity_ = { 0.0f,0.0f,0.0f };
		isAttack = true;
		behaviorRequest_ = Behavior::kAttack;
	}
}

//攻撃行動初期化
void Enemy::BehaviorAttackInitialize() {}
//攻撃行動更新
void Enemy::BehaviorAttackUpdate() {
	//tex_ = textures_[2]; 
	isAttack = false;
	BehaviorSetRoot();
}

void Enemy::BehaviorSetJump() {
	if (!isJump) {
		isJump = true;
		behaviorRequest_ = Behavior::kJump;
	}
}

void Enemy::BehaviorJumpInitialize(){
	//加速度ベクトル
	accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	//ジャンプ初速を与える
	velocity_.y = kJumpFirstSpeed;

	worldTransform_.translation_.y = 0.0f;
}

void Enemy::BehaviorJumpUpdate(){
	//移動
	velocity_.y += accelerationVector.y;
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	//着地
	if (worldTransform_.translation_.y <= 0.0f) {
		worldTransform_.translation_.y = 0.0f;
		//ジャンプ終了
		isJump = false;
		velocity_.y = 0.0f;
		worldTransform_.translation_.y = 0.0f;
		behaviorRequest_ = Behavior::kRoot;
		BehaviorSetRoot();
	}

	worldTransform_.UpdateMatrix();
}

void Enemy::BehaviorSetHit() {
	if (!isHit) {
		isHit = true;
		behaviorRequest_ = Behavior::kHit;
	}
}

//被弾時行動初期化
void Enemy::BehaviorHitInitialize() {}
//被弾時行動更新
void Enemy::BehaviorHitUpdate() {
	//死ぬ
	isDead_ = true;
	isHit = false;
	BehaviorSetRoot();
}

void Enemy::Reset()
{
	tex_ = textures_[1];

	//base
	worldTransform_.Initialize();
	worldTransform_ = worldTransformTemp_;
	worldTransform_.UpdateMatrix();

	isDead_ = false;
	isHit = false;
	isJump = false;
	isAttack = false;
		
	velocity_ = { 0.0f,0.0f,0.0f };

	BehaviorSetRoot();
}

void Enemy::Save(){
	worldTransformTemp_ = worldTransform_;
	worldTransformTemp_.UpdateMatrix();
}

void Enemy::Move(){
	//移動処理
	const float speed = 0.1f;
	//移動量に速さを反映
	velocity_ = Multiply(speed, Normalize(velocity_));
	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	worldTransform_.UpdateMatrix();
}
