#include "Player.h"
#include "MyMath.h"
#include <cassert>
#include <iostream>
#include "ImGuiManager.h"
#include "GlobalVariables.h"

//調整項目の適用
void Player::ApplyGlobalVariables() {
	//GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//const char* groupName = "Player";
	/*worldTransformFace_.translation_ = globalVariables->GetVector3Value(groupName, "Head Translation");
	worldTransformL_arm_.translation_ = globalVariables->GetVector3Value(groupName, "ArmL Translation");
	worldTransformR_arm_.translation_ = globalVariables->GetVector3Value(groupName, "ArmR Translation");
	floatingCycle_ = globalVariables->GetIntValue(groupName, "floatingCycle");
	floatingAmplitude_ = globalVariables->GetFloatValue(groupName, "floatingAmplitude");
	idelArmAngleMax_ = globalVariables->GetFloatValue(groupName, "idelArmAngleMax");*/
}

Player::Player() {}

Player::~Player() {}

void Player::Initialize(const std::vector<Model*>& models) {
	//既定クラスの初期化
	BaseCharacter::Initialize(models);

	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformFace_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformWeapon_.Initialize();

	worldTransformBody_.parent_ = &worldTransformBase_;
	worldTransformFace_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformWeapon_.parent_ = &worldTransformBody_;

	worldTransformL_arm_.translation_.x = worldTransformBase_.translation_.x + 0.75f;
	worldTransformR_arm_.translation_.x = worldTransformBase_.translation_.x - 0.75f;

	worldTransformWeapon_.translation_.y = worldTransformBase_.translation_.x - 1.5f;

	//浮遊ギミック初期化
	InitializeFloatingGimmick();

	//GlobalVariables* grobalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	/*grobalVariables->AddItem(groupName, "Head Translation", worldTransformFace_.translation_);
	grobalVariables->AddItem(groupName, "ArmL Translation", worldTransformL_arm_.translation_);
	grobalVariables->AddItem(groupName, "ArmR Translation", worldTransformR_arm_.translation_);
	grobalVariables->AddItem(groupName, "floatingCycle", floatingCycle_);
	grobalVariables->AddItem(groupName, "floatingAmplitude", floatingAmplitude_);
	grobalVariables->AddItem(groupName, "idelArmAngleMax", idelArmAngleMax_);*/
}

//浮遊ギミック初期化
void Player::InitializeFloatingGimmick() {
	//浮遊移動のサイクル<frame>
	floatingCycle_ = 60;
	//浮遊の振幅<m>
	floatingAmplitude_ = 0.5f;

	idelArmAngleMax_ = 0.0f;
}

//通常行動初期化
void Player::BehaviorRootInitialize() {
	worldTransformWeapon_.rotation_.x = -1.5f;
}

//攻撃行動初期化
void Player::BehaviorAttackInitialize() {
	afterAttackStay = 20;
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
	}
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
	worldTransformBase_.translation_.y = std::sin(idelArmAngleMax_) * floatingAmplitude_ + 5.0f;
	worldTransformL_arm_.rotation_.x = std::sin(idelArmAngleMax_) * floatingAmplitude_;
	worldTransformR_arm_.rotation_.x = -std::sin(idelArmAngleMax_) * floatingAmplitude_;

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Player")) {
		ImGui::SliderFloat3("face.translation", &worldTransformFace_.translation_.x, -10, 10);
		ImGui::SliderFloat3("body.translation", &worldTransformBody_.translation_.x, -10, 10);
		ImGui::SliderFloat3("L_arm.translation", &worldTransformL_arm_.translation_.x, -10, 10);
		ImGui::SliderFloat3("R_arm.translation", &worldTransformR_arm_.translation_.x, -10, 10);
		ImGui::SliderFloat3("weapon.translation_", &worldTransformWeapon_.translation_.x, -10, 10);
		ImGui::SliderFloat3("weapon.rotation_", &worldTransformWeapon_.rotation_.x, -10, 10);
		ImGui::SliderInt("period", reinterpret_cast<int*>(&floatingCycle_), 1, 144);
		ImGui::SliderFloat("amplitude", &floatingAmplitude_, 0.0f, PAI * 2.0f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}

//通常行動更新
void Player::BehaviorRootUpdate() {
	//既定クラスの更新
	BaseCharacter::Update();

	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	//Rトリガーを押していたら
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			isAttack = true;
		}

		if (isAttack) {
			if (worldTransformWeapon_.rotation_.x <= 0.6f) {
				worldTransformWeapon_.rotation_.x += 0.1f;
			}
			else if (worldTransformWeapon_.rotation_.x >= 0.6f) {
				behaviorRequest_ = Behavior::kAttack;
			}
		}
		else {
			//浮遊ギミック更新
			UpdateFloatingGimmick();

			//速さ
			const float speed = 0.3f;
			//移動量
			Vector3 move = {
				(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
				0,
				(float)joyState.Gamepad.sThumbLY / SHRT_MAX
			};
			//移動量に速さを反映
			move = Multiply(speed, Normalize(move));

			//移動ベクトルをカメラの角度だけ回転する
			Matrix4x4 rotate = MakeRotateMatrix(viewProjection_->rotation_);
			move = TransformNormal(move, rotate);

			//移動方向に向きを合わせる
			//Y軸周り角度(θy)
			worldTransformBase_.rotation_.y = std::atan2(-move.x, -move.z);

			//移動
			worldTransformBase_.translation_ = Add(worldTransformBase_.translation_, move);
		}
	}

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformFace_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformWeapon_.UpdateMatrix();

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Player")) {
		ImGui::SliderFloat3("face.translation", &worldTransformFace_.translation_.x, -10, 10);
		ImGui::SliderFloat3("body.translation", &worldTransformBody_.translation_.x, -10, 10);
		ImGui::SliderFloat3("L_arm.translation", &worldTransformL_arm_.translation_.x, -10, 10);
		ImGui::SliderFloat3("R_arm.translation", &worldTransformR_arm_.translation_.x, -10, 10);
		ImGui::SliderFloat3("weapon.translation_", &worldTransformWeapon_.translation_.x, -10, 10);
		ImGui::SliderFloat3("weapon.rotation_", &worldTransformWeapon_.rotation_.x, -10, 10);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}


//攻撃行動更新
void Player::BehaviorAttackUpdate() {
	//既定クラスの更新
	BaseCharacter::Update();

	if (worldTransformWeapon_.rotation_.x >= -1.5f) {
		worldTransformWeapon_.rotation_.x -= 0.1f;
	}
	else if (worldTransformWeapon_.rotation_.x <= -1.5f) {
		afterAttackStay--;
		if (afterAttackStay <= 0) {
			isAttack = false;
			behaviorRequest_ = Behavior::kRoot;
		}
	}

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformFace_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformWeapon_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {
	//既定クラスの描画
	//BaseCharacter::Draw(viewProjection);

	models_[kModelFace]->Draw(worldTransformFace_, viewProjection);
	models_[kModelBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelR_arm]->Draw(worldTransformR_arm_, viewProjection);
	if (isAttack) {
		models_[kModelWeapon]->Draw(worldTransformWeapon_, viewProjection);
	}
}