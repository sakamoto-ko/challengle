#include "Player.h"

#include "MyMath.h"

#include <cassert>
#include <list>

#include "GameScene.h"

Player::Player() {

}
Player::~Player() {
}

Vector3 Player::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos = {};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

//親となるトランスフォームをセット
void Player::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
void Player::Move() {
	XINPUT_STATE joyState;

	//速さ
	const float speed = 0.1f;
	//移動量
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		velocity_.x = (float)joyState.Gamepad.sThumbLX / SHRT_MAX;
	}

	//移動量に速さを反映
	velocity_ = Multiply(speed, Normalize(velocity_));

	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	worldTransform_.UpdateMatrix();
}

// 初期化
void Player::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// キー入力 初期化
	button->Initialize();

	SetModels(models);
	SetTextures(textures);
	tex_ = textures_[0];

	//base
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f,0.0f,4.0f };
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.scale_ = { 0.2f,0.2f,0.2f };
	worldTransform_.UpdateMatrix();
}

// 更新
void Player::Update(const ViewProjection viewProjection) {

	assert(gameScene_);

	//キー入力の更新
	button->Update();

	Move();

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
	if (ImGui::TreeNode("player")) {
		if (ImGui::TreeNode("base")) {
			ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("behavior")) {
		ImGui::Text("behavior %d", behavior_);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	worldTransform_.UpdateMatrix();
}

// 描画
void Player::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransform_, viewProjection, tex_);
}

//UI描画
void Player::DrawUI() {}

//通常行動初期化
void Player::BehaviorRootInitialize() {
	//worldTransformWeapon_.rotation_.x = -1.5f;
}
//通常行動更新
void Player::BehaviorRootUpdate() {
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//Rトリガーを押していたら
		if (button->isTriggerAttack()) {
			//isAttack = true;
			/*if (isAttack) {
			if (worldTransformWeapon_.rotation_.x <= 0.6f) {
				worldTransformWeapon_.rotation_.x += 0.1f;
			}
			else if (worldTransformWeapon_.rotation_.x >= 0.6f) {
				behaviorRequest_ = Behavior::kAttack;
			}
		}*/
		}

		//ジャンプボタンを押したら
		if (button->isTriggerJump()) {
			if (!isJump) {
				isJump = true;
			}
		}
		if (isJump) {
			behaviorRequest_ = Behavior::kJump;
		}

		Move();
	}

	worldTransform_.UpdateMatrix();
}

//攻撃行動初期化
void Player::BehaviorAttackInitialize() {
	//afterAttackStay = 20;
}
//攻撃行動更新
void Player::BehaviorAttackUpdate() {
	/*if (worldTransformWeapon_.rotation_.x >= -1.5f) {
		worldTransformWeapon_.rotation_.x -= 0.1f;
	}
	else if (worldTransformWeapon_.rotation_.x <= -1.5f) {
		afterAttackStay--;
		if (afterAttackStay <= 0) {
			isAttack = false;
			behaviorRequest_ = Behavior::kRoot;
		}
	}*/
}

//ジャンプ行動初期化
void Player::BehaviorJumpInitialize() {
	//加速度ベクトル
	accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	//ジャンプ初速を与える
	velocity_.y = kJumpFirstSpeed;

	worldTransform_.translation_.y = 0.0f;
}
//ジャンプ行動更新
void Player::BehaviorJumpUpdate() {

	//着地
	if (worldTransform_.translation_.y <= 0.0f) {
		worldTransform_.translation_.y = 0.0f;
		//ジャンプ終了
		isJump = false;
		velocity_.y = 0.0f;
		worldTransform_.translation_.y = 0.0f;
		behaviorRequest_ = Behavior::kRoot;
	}
	else {
		//加速する
		velocity_ = Add(velocity_, accelerationVector);
		//移動
		worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("jump")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
		ImGui::DragFloat3("velocity", &velocity_.x, 0.01f);
		ImGui::DragFloat3("acceleration", &accelerationVector.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	worldTransform_.UpdateMatrix();
}