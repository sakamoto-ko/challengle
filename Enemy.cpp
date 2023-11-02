#include "Enemy.h"

#include "MyMath.h"

#include <cassert>
#include <list>
#include <iostream>
#include <random>
#include <iomanip>

#include "GameScene.h"

Enemy::Enemy() {

}
Enemy::~Enemy() {
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
	hp -= 20;
	isHit = true;
	behaviorRequest_ = Behavior::kHit;
}

//親となるトランスフォームをセット
void Enemy::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

// 初期化
void Enemy::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// キー入力 初期化
	button->Initialize();

	SetModels(models);
	SetTextures(textures);
	tex_ = textures_[1];
	bulletModels_ = models;
	bulletTextures_ = textures;
	bulletTex_ = textures_[1];

	//base
	worldTransform_.Initialize();
	worldTransform_.translation_ = { moveMax.x,moveMax.y,32.5f };
	worldTransform_.rotation_ = { 0.0f,-4.090f,0.0f };
	worldTransform_.scale_ = { 4.0f,4.0f,4.0f };
	worldTransform_.UpdateMatrix();
	// 体
	worldTransformBody_.Initialize();
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformBody_.translation_ = { 0.0f,0.0f,0.0f };
	worldTransformBody_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformBody_.scale_ = { 0.3f,0.6f,0.3f };
	worldTransformBody_.UpdateMatrix();
	// 頭
	worldTransformHead_.Initialize();
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformHead_.translation_ = { 0.0f,1.7f,0.0f };
	worldTransformHead_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformHead_.scale_ = { 0.8f,0.5f,0.8f };
	worldTransformHead_.UpdateMatrix();
	// 左手
	worldTransformArmL_.Initialize();
	worldTransformArmL_.parent_ = &worldTransformBody_;
	worldTransformArmL_.translation_ = { 0.0f,0.0f,-2.1f };
	worldTransformArmL_.rotation_ = { 0.56f,0.0f,0.0f };
	worldTransformArmL_.scale_ = { 0.4f,0.8f,0.4f };
	worldTransformArmL_.UpdateMatrix();
	// 右手
	worldTransformArmR_.Initialize();
	worldTransformArmR_.parent_ = &worldTransformBody_;
	worldTransformArmR_.translation_ = { 0.0f,0.0f,2.1f };
	worldTransformArmR_.rotation_ = { -0.56f,0.0f,0.0f };
	worldTransformArmR_.scale_ = { 0.4f,0.8f,0.4f };
	worldTransformArmR_.UpdateMatrix();
	// 左足
	worldTransformLegL_.Initialize();
	worldTransformLegL_.parent_ = &worldTransformBody_;
	worldTransformLegL_.translation_ = { 0.0f,-2.05f,-0.72f };
	worldTransformLegL_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformLegL_.scale_ = { 0.34f,1.0f,0.34f };
	worldTransformLegL_.UpdateMatrix();
	// 右足
	worldTransformLegR_.Initialize();
	worldTransformLegR_.parent_ = &worldTransformBody_;
	worldTransformLegR_.translation_ = { 0.0f,-2.05f,0.72f };
	worldTransformLegR_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformLegR_.scale_ = { 0.34f,1.0f,0.34f };
	worldTransformLegR_.UpdateMatrix();
	// 武器
	worldTransformWeapon_.Initialize();
	worldTransformWeapon_.parent_ = &worldTransformBody_;
	worldTransformWeapon_.translation_ = { 1.84f,-0.55f,2.46f };
	worldTransformWeapon_.rotation_ = { 0.0f,0.0f,-1.0f };
	worldTransformWeapon_.scale_ = { 0.2f,3.5f,0.2f };
	worldTransformWeapon_.UpdateMatrix();

	hp = hp_start;
	isDead_ = false;

	InitializeFloatingGimmick();

	behaviorRequest_ = Behavior::kRoot;
}

// 更新
void Enemy::Update(const ViewProjection viewProjection) {

	assert(gameScene_);

	//XINPUT_STATE joyState;

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

		case Behavior::kHalfHp:
			BehaviorHalfHpInitialize();
			break;

		case Behavior::kAttack1:
			BehaviorAttack1Initialize();
			break;

		case Behavior::kAttack2:
			BehaviorAttack2Initialize();
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

	case Behavior::kHalfHp:
		BehaviorHalfHpUpdate();
		break;

	case Behavior::kAttack1:
		BehaviorAttack1Update();
		break;

	case Behavior::kAttack2:
		BehaviorAttack2Update();
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
		if (ImGui::TreeNode("body")) {
			ImGui::DragFloat3("translation", &worldTransformBody_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransformBody_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransformBody_.scale_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("head")) {
			ImGui::DragFloat3("translation", &worldTransformHead_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransformHead_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransformHead_.scale_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("armL")) {
			ImGui::DragFloat3("translation", &worldTransformArmL_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransformArmL_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransformArmL_.scale_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("armR")) {
			ImGui::DragFloat3("translation", &worldTransformArmR_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransformArmR_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransformArmR_.scale_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("LegL")) {
			ImGui::DragFloat3("translation", &worldTransformLegL_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransformLegL_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransformLegL_.scale_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("legR")) {
			ImGui::DragFloat3("translation", &worldTransformLegR_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransformLegR_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransformLegR_.scale_.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::Text("hp %d", hp);
		ImGui::DragFloat2("moveMax.x", &moveMax.x, 0.01f);
		ImGui::DragFloat2("moveMax.y", &moveMax.y, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}

// 描画
void Enemy::Draw(const ViewProjection& viewProjection) {
	if (!isDead_) {
		models_[BODY]->Draw(worldTransformBody_, viewProjection, tex_);
		models_[HEAD]->Draw(worldTransformHead_, viewProjection, tex_);
		models_[ARM_L]->Draw(worldTransformArmL_, viewProjection, tex_);
		models_[ARM_R]->Draw(worldTransformArmR_, viewProjection, tex_);
		models_[LEG_L]->Draw(worldTransformLegL_, viewProjection, tex_);
		models_[LEG_R]->Draw(worldTransformLegR_, viewProjection, tex_);
	}
}

//UI描画
void Enemy::DrawUI() {
}

//通常行動初期化
void Enemy::BehaviorRootInitialize() {
	velocity_ = { 0.0f,0.0f,0.0f };
	moveTime = 0;
	isAttack = false;
	isAttack2 = false;
}
//通常行動更新
void Enemy::BehaviorRootUpdate() {

	tex_ = textures_[1];

	//攻撃
	if (isHalfHp) {
		if (++attackWaitCount >= 180) {
			isAttack = true;
			behaviorRequest_ = Behavior::kAttack1;
			velocity_ = { 0.0f,0.0f,0.0f };
			attackWaitCount = 0;
		}
	}
	if (++attack2WaitCount >= 120) {
		isAttack2 = true;
		behaviorRequest_ = Behavior::kAttack2;
		velocity_ = { 0.0f,0.0f,0.0f };
		attack2WaitCount = 0;
	}

	//ランダムな浮動小数点の生成
	std::random_device v;
	std::default_random_engine eng(v());
	std::uniform_real_distribution<float> distr(-10.0f, 10.0f);

	//移動処理
	//速さ
	if (isHalfHp) {
		speed = 0.8f;
	}
	else {
		speed = 0.5f;
	}
	if (++moveTime >= 15) {
		//移動量
		velocity_.x = distr(eng);
		velocity_.y = distr(eng);
		moveTime = 0;
	}
	//移動量に速さを反映
	velocity_ = Multiply(speed, Normalize(velocity_));
	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	UpdateFloatingGimmick();

	//移動範囲上限
	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, moveMin.x, moveMax.x);
	worldTransform_.translation_.y = Clamp(worldTransform_.translation_.y, moveMin.y, moveMax.y);
	if (worldTransform_.translation_.x >= moveMax.x ||
		worldTransform_.translation_.x <= moveMin.x) {
		velocity_.x *= -1;
	}
	if (worldTransform_.translation_.y >= moveMax.y ||
		worldTransform_.translation_.y <= moveMin.y) {
		velocity_.y *= -1;
	}
	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformArmL_.UpdateMatrix();
	worldTransformArmR_.UpdateMatrix();
	worldTransformLegL_.UpdateMatrix();
	worldTransformLegR_.UpdateMatrix();
	worldTransformWeapon_.UpdateMatrix();
}

//攻撃1行動初期化
void Enemy::BehaviorAttack1Initialize() {
	attackCount = 0;
	attackNum = 1;
	bulletVelocity[0] = { -1.0f,0.0f,0.0f };
}
//攻撃1行動更新
void Enemy::BehaviorAttack1Update() {
	if (++attackCount >= 60) {
		behaviorRequest_ = Behavior::kRoot;
		isAttack = false;
		attackCount = 0;
	}
	Shot();
}

//攻撃2行動初期化
void Enemy::BehaviorAttack2Initialize() {
	attackNum = 2;
	if (isHalfHp) {
		bulletVelocity[0] = { -0.6f,0.4f,0.0f };
		bulletVelocity[1] = { -0.6f,-0.4f,0.0f };
	}
	else {
		bulletVelocity[0] = { -0.6f,0.4f,0.0f };
		bulletVelocity[1] = { -0.6f,-0.4f,0.0f };
	}
}
//攻撃2行動更新
void Enemy::BehaviorAttack2Update() {
	Shot();
	behaviorRequest_ = Behavior::kRoot;
	isAttack2 = false;
}
//被弾時行動初期化
void Enemy::BehaviorHitInitialize() {
	hitTime = 0;
}
//被弾時行動更新
void Enemy::BehaviorHitUpdate() {

	tex_ = textures_[2];

	if (isHit) {
		//死ぬ
		if (hp <= 0) {
			isDead_ = true;
		}
		//半分時行動
		else if (hp <= hp_start / 2) {
			if (!isHalfHp) {
				behaviorRequest_ = Behavior::kHalfHp;
			}
		}

		//通常被弾時行動
		if (++hitTime >= 20) {
			isHit = false;
			hitTime = 0;
			behaviorRequest_ = Behavior::kRoot;
		}
	}

}
//体力半分時行動初期化
void Enemy::BehaviorHalfHpInitialize() {
	halfHpCount = 0;
	velocity_ = { 0.0f,0.0f,0.0f };
}
//体力半分時行動更新
void Enemy::BehaviorHalfHpUpdate() {

	//ランダムな浮動小数点の生成
	std::random_device v;
	std::default_random_engine eng(v());
	std::uniform_real_distribution<float> distr(-10.0f, 10.0f);

	//移動処理
	//速さ
	speed = 0.1f;
	//移動量
	velocity_.x = distr(eng);
	velocity_.y = distr(eng);
	//移動量に速さを反映
	velocity_ = Multiply(speed, Normalize(velocity_));
	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	//移動範囲上限
	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, moveMin.x, moveMax.x);
	worldTransform_.translation_.y = Clamp(worldTransform_.translation_.y, moveMin.y, moveMax.y);
	if (worldTransform_.translation_.x >= moveMax.x ||
		worldTransform_.translation_.x <= moveMin.x) {
		velocity_.x *= -0.8f;
	}
	if (worldTransform_.translation_.y >= moveMax.y ||
		worldTransform_.translation_.y <= moveMin.y) {
		velocity_.y *= -0.8f;
	}

	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformArmL_.UpdateMatrix();
	worldTransformArmR_.UpdateMatrix();
	worldTransformLegL_.UpdateMatrix();
	worldTransformLegR_.UpdateMatrix();
	worldTransformWeapon_.UpdateMatrix();

	if (++halfHpCount >= 60) {
		isHalfHp = true;
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Enemy::Reset()
{
	tex_ = textures_[1];

	//base
	worldTransform_.Initialize();
	worldTransform_.translation_ = { moveMax.x,moveMax.y,32.5f };
	worldTransform_.rotation_ = { 0.0f,-4.090f,0.0f };
	worldTransform_.scale_ = { 4.0f,4.0f,4.0f };
	worldTransform_.UpdateMatrix();
	// 体
	worldTransformBody_.Initialize();
	worldTransformBody_.translation_ = { 0.0f,0.0f,0.0f };
	worldTransformBody_.rotation_ = { 0.0f,0.0f,-0.29f };
	worldTransformBody_.scale_ = { 0.3f,0.6f,0.3f };
	worldTransformBody_.UpdateMatrix();
	// 頭
	worldTransformHead_.Initialize();
	worldTransformHead_.translation_ = { 0.0f,1.7f,0.0f };
	worldTransformHead_.rotation_ = { 0.0f,0.0f,-0.08f };
	worldTransformHead_.scale_ = { 0.8f,0.5f,0.8f };
	worldTransformHead_.UpdateMatrix();
	// 左手
	worldTransformArmL_.Initialize();
	worldTransformArmL_.translation_ = { 0.0f,0.0f,-2.1f };
	worldTransformArmL_.rotation_ = { 0.56f,0.0f,0.0f };
	worldTransformArmL_.scale_ = { 0.4f,0.8f,0.4f };
	worldTransformArmL_.UpdateMatrix();
	// 右手
	worldTransformArmR_.Initialize();
	worldTransformArmR_.translation_ = { 0.0f,0.0f,2.1f };
	worldTransformArmR_.rotation_ = { -0.56f,0.0f,0.0f };
	worldTransformArmR_.scale_ = { 0.4f,0.8f,0.4f };
	worldTransformArmR_.UpdateMatrix();
	// 左足
	worldTransformLegL_.Initialize();
	worldTransformLegL_.translation_ = { -0.48f,-2.05f,-0.72f };
	worldTransformLegL_.rotation_ = { 0.0f,0.0f,-0.55f };
	worldTransformLegL_.scale_ = { 0.34f,1.0f,0.34f };
	worldTransformLegL_.UpdateMatrix();
	// 右足
	worldTransformLegR_.Initialize();
	worldTransformLegR_.translation_ = { -0.2f,-2.05f,0.72f };
	worldTransformLegR_.rotation_ = { 0.0f,0.0f,-0.55f };
	worldTransformLegR_.scale_ = { 0.34f,1.0f,0.34f };
	worldTransformLegR_.UpdateMatrix();
	// 武器
	worldTransformWeapon_.Initialize();
	worldTransformWeapon_.translation_ = { 1.84f,0.55f,2.46f };
	worldTransformWeapon_.rotation_ = { 0.0f,0.0f,-1.0f };
	worldTransformWeapon_.scale_ = { 0.2f,3.5f,0.2f };
	worldTransformWeapon_.UpdateMatrix();

	hp = hp_start;
	isDead_ = false;
	isHalfHp = false;
	halfHpCount = 0;
	isAttack = false;
	isAttack2 = false;
	attackCount = 0;
	attackNum = 0;
	velocity_ = { 0.0f,0.0f,0.0f };
	attackWaitCount = 0;
	moveTime = 0;

	InitializeFloatingGimmick();

	behaviorRequest_ = Behavior::kRoot;
}

void Enemy::Shot() {
	if (isAttack) {
		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(bulletModels_, bulletTextures_, GetWorldPosition(), bulletVelocity[0]);
		newBullet->SetAttackNum(attackNum);
		gameScene_->AddEnemyBullet(newBullet);
	}
	if (isAttack2) {
		EnemyBullet* newBullet[2] = { new EnemyBullet(),new EnemyBullet() };
		for (int i = 0; i < attackNum; i++) {
			newBullet[i]->Initialize(bulletModels_, bulletTextures_, GetWorldPosition(), bulletVelocity[i]);
			newBullet[i]->SetAttackNum(attackNum);
			gameScene_->AddEnemyBullet(newBullet[i]);
		}
	}
}

void Enemy::InitializeFloatingGimmick() {
	floatingParameter_ = 0.0f;
}

void Enemy::UpdateFloatingGimmick() {
	//浮遊移動のサイクル
	const uint16_t period = 60;

	//1フレームでのパラメーター加算値
	const float step = 2.0f * PAI / period;

	//パラメータを1ステップ分加算
	floatingParameter_ += step;

	//2πを越えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * PAI);

	//浮遊の振幅＜ｍ＞
	const float SW = 0.5f;

	//浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * SW;
	worldTransformArmL_.rotation_.z = std::cos(floatingParameter_) * SW;
	worldTransformArmR_.rotation_.z = -std::cos(floatingParameter_) * SW;
}
