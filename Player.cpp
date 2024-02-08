#include "Player.h"
#include "MyMath.h"
#include <cassert>
#include <iostream>
#include "ImGuiManager.h"
#include "GlobalVariables.h"
#include "LockOn.h"

//調整項目の適用
void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	worldTransformFace_.translation_ = globalVariables->GetVector3Value(groupName, "Head Translation");
	worldTransformL_arm_.translation_ = globalVariables->GetVector3Value(groupName, "ArmL Translation");
	worldTransformR_arm_.translation_ = globalVariables->GetVector3Value(groupName, "ArmR Translation");
	/*floatingCycle_ = globalVariables->GetIntValue(groupName, "floatingCycle");
	floatingAmplitude_ = globalVariables->GetFloatValue(groupName, "floatingAmplitude_");
	idelArmAngleMax_ = globalVariables->GetFloatValue(groupName, "idelArmAngleMax_");*/
}

void Player::AllWorldTransformUpdateMatrix()
{
	//既定クラスの更新
	BaseCharacter::Update();
	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformFace_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::SetBehaviorRoot()
{
	behaviorRequest_ = Behavior::kRoot;
	isAttack_ = false;
}

void Player::Reset(const std::vector<Model*>& models)
{
	//既定クラスの初期化
	BaseCharacter::Initialize(models);

	//ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_.y = 3.0f;
	//体
	worldTransformBody_.Initialize();
	worldTransformBody_.parent_ = &worldTransform_;
	//頭
	worldTransformFace_.Initialize();
	worldTransformFace_.parent_ = &worldTransformBody_;
	//左腕
	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	//右腕
	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	worldTransformL_arm_.translation_.x = worldTransform_.translation_.x + 0.75f;
	worldTransformR_arm_.translation_.x = worldTransform_.translation_.x - 0.75f;

	velocity_ = { 0.0f,0.0f,0.0f };

	//浮遊ギミック初期化
	InitializeFloatingGimmick();

	isAttack_ = false;
	isJump_ = false;

	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));
}

void Player::OnCollision([[maybe_unused]] Collider* other)
//void Player::OnCollision()
{
	//衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		//ジャンプリクエスト
		SetBehavior(Behavior::kJump);
		isJump_ = true;
	}
}

Player::Player() {}

Player::~Player() {}

void Player::Initialize(const std::vector<Model*>& models) {
	GlobalVariables* grobalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	grobalVariables->AddItem(groupName, "Head Translation", worldTransformFace_.translation_);
	grobalVariables->AddItem(groupName, "ArmL Translation", worldTransformL_arm_.translation_);
	grobalVariables->AddItem(groupName, "ArmR Translation", worldTransformR_arm_.translation_);

	ApplyGlobalVariables();

	Reset(models);
}

Vector3 Player::GetCenterPosition() const
{
	//const Vector3 offset = { 0.0f,1.5f,0.0f };

	const Vector3 offset = { 0.0f,0.0f,0.0f };

	Vector3 worldPos = TransformNormal(offset, worldTransform_.matWorld_);

	return worldPos;
}

//浮遊ギミック初期化
void Player::InitializeFloatingGimmick() {
	//ApplyGlobalVariables();

	//浮遊移動のサイクル<frame>
	floatingCycle_ = 60;
	//浮遊の振幅<m>
	floatingAmplitude_ = 0.5f;

	idelArmAngleMax_ = 0.0f;
}

//通常行動初期化
void Player::BehaviorRootInitialize() {
}

//攻撃行動初期化
void Player::BehaviorAttackInitialize() {
}

void Player::Update() {
	if (behaviorRequest_) {
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞い事の初期化を実行
		switch (behavior_) {
		case Player::Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Player::Behavior::kJump:
			BehaviorJumpInitialize();
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
		//攻撃行動
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
		//ジャンプ行動
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Player")) {
		if (ImGui::TreeNode("face")) {
			ImGui::SliderFloat3("face.translation", &worldTransformFace_.translation_.x, -10, 10);
			ImGui::SliderFloat3("face.rotation_", &worldTransformFace_.rotation_.x, -10, 10);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("body")) {
			ImGui::SliderFloat3("body.translation", &worldTransformBody_.translation_.x, -10, 10);
			ImGui::SliderFloat3("body.rotation_", &worldTransformBody_.rotation_.x, -10, 10);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("L_arm")) {
			ImGui::SliderFloat3("L_arm.translation", &worldTransformL_arm_.translation_.x, -10, 10);
			ImGui::DragFloat3("L_arm.rotation_", &worldTransformL_arm_.rotation_.x, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("R_arm")) {
			ImGui::SliderFloat3("R_arm.translation", &worldTransformR_arm_.translation_.x, -10, 10);
			ImGui::DragFloat3("R_arm.rotation_", &worldTransformR_arm_.rotation_.x, 0.1f);
			ImGui::TreePop();
		}
		/*if (ImGui::TreeNode("weapon")) {
			ImGui::SliderFloat3("weapon.translation_", &worldTransformWeapon_.translation_.x, -10, 10);
			ImGui::SliderFloat3("weapon.rotation_", &worldTransformWeapon_.rotation_.x, -10, 10);
			ImGui::TreePop();
		}*/
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	AllWorldTransformUpdateMatrix();
}

//浮遊ギミック更新
void Player::UpdateFloatingGimmick() {
	//1フレームでのパラメータ加算値
	const float step = 2.0f * PAI / floatingCycle_;
	//パラメータを1ステップ分加算
	idelArmAngleMax_ += step;
	//2πを超えたら0に戻す
	idelArmAngleMax_ = std::fmod(idelArmAngleMax_, 2.0f * PAI);
	//浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sinf(idelArmAngleMax_) * floatingAmplitude_;
	worldTransformL_arm_.rotation_.x = std::sin(idelArmAngleMax_) * floatingAmplitude_;
	worldTransformR_arm_.rotation_.x = -std::sin(idelArmAngleMax_) * floatingAmplitude_;
}

//通常行動更新
void Player::BehaviorRootUpdate() {
	//既定クラスの更新
	BaseCharacter::Update();

	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//Rトリガーを押したら
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			isAttack_ = true;
		}
		//Aボタンを押したら
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			isJump_ = true;
		}
		//スティックの入力があったら
		if ((float)joyState.Gamepad.sThumbLX / SHRT_MAX != 0.0f ||
			(float)joyState.Gamepad.sThumbLY / SHRT_MAX != 0.0f) {
			//速さ
			const float speed = 0.3f;
			//移動量
			velocity_ = {
				(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
				0.0f,
				(float)joyState.Gamepad.sThumbLY / SHRT_MAX
			};

			//移動量に速さを反映
			velocity_ = Multiply(speed, Normalize(velocity_));

			//移動ベクトルをカメラの角度だけ回転する
			Matrix4x4 rotate = MakeRotateMatrix(viewProjection_->rotation_);
			velocity_ = TransformNormal(velocity_, rotate);

			//移動方向に向きを合わせる
			//Y軸周り角度(θy)
			worldTransform_.rotation_.y = std::atan2(-velocity_.x, -velocity_.z);

			//移動
			worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
		}
		//スティックの入力がないとき、ロックオンした敵を追従(視点)
		else if (lockOn_ && lockOn_->ExistTarget()) {
			Vector3 lockOnPosition = lockOn_->GetTargetPosition();
			Vector3 sub = Subtract(lockOnPosition, worldTransform_.translation_);

			worldTransform_.rotation_.y = std::atan2(-sub.x, -sub.z);
		}
	}

	//攻撃更新
	if (isAttack_) {
		SetBehavior(Behavior::kAttack);
	}
	//ジャンプ更新
	if (isJump_) {
		SetBehavior(Behavior::kJump);
	}

	//浮遊ギミック更新
	if (!isAttack_ && !isJump_) {
		UpdateFloatingGimmick();
	}

	AllWorldTransformUpdateMatrix();
}

//攻撃行動更新
void Player::BehaviorAttackUpdate() {
	//ロックオン中
	if (lockOn_ && lockOn_->ExistTarget()) {
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		Vector3 sub = Subtract(lockOnPosition, worldTransform_.translation_);

		//距離
		float distance = Length(sub);
		//距離閾値
		const float threshold = 0.2f;

		//閾値より離れている時のみ
		if (distance > threshold) {
			worldTransform_.rotation_.y = std::atan2(-sub.x, -sub.z);
		}
	}

	AllWorldTransformUpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {
	//既定クラスの描画
	//BaseCharacter::Draw(viewProjection);

	models_[kModelFace]->Draw(worldTransformFace_, viewProjection);
	models_[kModelBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelR_arm]->Draw(worldTransformR_arm_, viewProjection);
}

//ジャンプ行動初期化
void Player::BehaviorJumpInitialize() {
	worldTransformBody_.translation_.y = 0.0f;
	worldTransformL_arm_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformR_arm_.rotation_ = { 0.0f,0.0f,0.0f };

	//ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	//ジャンプ初速を与える
	velocity_.y = kJumpFirstSpeed;
}
//ジャンプ行動更新
void Player::BehaviorJumpUpdate() {

	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		velocity_.x = (float)joyState.Gamepad.sThumbLX / SHRT_MAX;
		velocity_.z = (float)joyState.Gamepad.sThumbLY / SHRT_MAX;

		//移動量に速さを反映
		velocity_.x = 0.3f * Normalize(velocity_.x);
		velocity_.z = 0.3f * Normalize(velocity_.z);

		//移動ベクトルをカメラの角度だけ回転する
		Matrix4x4 rotate = MakeRotateMatrix(viewProjection_->rotation_);
		velocity_ = TransformNormal(velocity_, rotate);

		//移動方向に向きを合わせる
		//Y軸周り角度(θy)
		worldTransform_.rotation_.y = std::atan2(-velocity_.x, -velocity_.z);

		//移動
		worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
		//重力加速度
		const float kGravityAcceleration = 0.05f;
		//加速度ベクトル
		Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
		//加速する
		velocity_.y += accelerationVector.y;

		//着地
		if (worldTransform_.translation_.y <= 3.0f) {
			worldTransform_.translation_.y = 3.0f;
			//ジャンプ終了
			isJump_ = false;
			SetBehavior(Behavior::kRoot);
		}
	}

	AllWorldTransformUpdateMatrix();
}
