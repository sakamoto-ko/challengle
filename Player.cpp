#include "Player.h"
#include "MyMath.h"
#include <cassert>
#include <iostream>
#include "ImGuiManager.h"
#include "GlobalVariables.h"
#include "GameScene.h"

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

Player::Player() {}

Player::~Player() {}

Vector3 Player::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos = {};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];

	return worldPos;
}

void Player::Initialize(const std::vector<Model*>& models) {
	//既定クラスの初期化
	BaseCharacter::Initialize(models);

	Reset();

	//bulletTextures_ = textures;
	bulletModels_ = models;
	//bulletTex_ = textures_[0];

	GlobalVariables* grobalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	ApplyGlobalVariables();

	grobalVariables->AddItem(groupName, "Head Translation", worldTransformFace_.translation_);
	grobalVariables->AddItem(groupName, "ArmL Translation", worldTransformL_arm_.translation_);
	grobalVariables->AddItem(groupName, "ArmR Translation", worldTransformR_arm_.translation_);
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
	playerBulletNum_ = 0;
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
		//Rトリガーを押していたら
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ||
			joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			isAttack = true;
		}

		//ジャンプボタンを押したら
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			isJump = true;
		}

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
		worldTransformBase_.rotation_.y = std::atan2(-velocity_.x, -velocity_.z);

		//移動
		worldTransformBase_.translation_ = Add(worldTransformBase_.translation_, velocity_);

		//攻撃更新
		if (isAttack) {
			behaviorRequest_ = Behavior::kAttack;
		}

		//ジャンプ更新
		if (isJump) {
			behaviorRequest_ = Behavior::kJump;
		}

		//浮遊ギミック更新
		if (!isAttack && !isJump) {
			UpdateFloatingGimmick();
		}
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Player")) {
		ImGui::SliderFloat3("translation", &worldTransformBase_.translation_.x, -10, 10);
		ImGui::SliderFloat3("reticle", &worldTransformReticle_.translation_.x, -10, 10);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	//行動制限
	worldTransformBase_.translation_.x = Clamp(worldTransformBase_.translation_.x, -50.0f, 50.0f);
	worldTransformBase_.translation_.z = Clamp(worldTransformBase_.translation_.z, -55.0f, 55.0f);

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformFace_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

//攻撃行動更新
void Player::BehaviorAttackUpdate() {
	//既定クラスの更新
	BaseCharacter::Update();

	assert(gameScene_);

	if (playerBulletNum_ < 1) {

		//弾の速度
		const float kBulletSpeed = 0.1f;

		Vector3 bulletVelocity = {};

		//自機から照準オブジェクトへのベクトル
		bulletVelocity = Subtract(worldTransformReticle_.translation_, worldTransformBase_.translation_);
		bulletVelocity = Multiply(kBulletSpeed, bulletVelocity);

		//弾を生成し、初期化
		playerBulletNum_++;
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(
			bulletModels_,
			{ worldTransformBase_.matWorld_.m[3][0],worldTransformBase_.matWorld_.m[3][1],worldTransformBase_.matWorld_.m[3][2] }
		, bulletVelocity);

		//弾をゲームシーンに登録する
		gameScene_->AddPlayerBullet(newBullet);
	}

	isAttack = false;
	behaviorRequest_ = Behavior::kRoot;

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Player")) {
		ImGui::SliderFloat3("translation", &worldTransformBase_.translation_.x, -10, 10);
		ImGui::SliderFloat3("reticle", &worldTransformReticle_.translation_.x, -10, 10);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformFace_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {
	//既定クラスの描画
	//BaseCharacter::Draw(viewProjection);

	if (!isDead_) {
		models_[kModelFace]->Draw(worldTransformFace_, viewProjection);
		models_[kModelBody]->Draw(worldTransformBody_, viewProjection);
		models_[kModelL_arm]->Draw(worldTransformL_arm_, viewProjection);
		models_[kModelR_arm]->Draw(worldTransformR_arm_, viewProjection);
	}
}

//ジャンプ行動初期化
void Player::BehaviorJumpInitialize() {
	worldTransformBody_.translation_.y = 0.0f;
	worldTransformL_arm_.rotation_.x = 0.0f;
	worldTransformR_arm_.rotation_.x = 0.0f;

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
		worldTransformBase_.rotation_.y = std::atan2(-velocity_.x, -velocity_.z);

		//移動
		worldTransformBase_.translation_ = Add(worldTransformBase_.translation_, velocity_);
		//重力加速度
		const float kGravityAcceleration = 0.05f;
		//加速度ベクトル
		Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
		//加速する
		velocity_.y += accelerationVector.y;

		//着地
		if (worldTransformBase_.translation_.y <= 3.0f) {
			worldTransformBase_.translation_.y = 3.0f;
			//ジャンプ終了
			isJump = false;
			behaviorRequest_ = Behavior::kRoot;
		}
	}

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformFace_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Player")) {
		ImGui::SliderFloat3("translation", &worldTransformBase_.translation_.x, -10, 10);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}

void Player::GetReticlePosition(const ViewProjection viewProjection) {
	if (spritePos_ != nullptr) {
		//スプライトの現在座標を取得
		Vector2 spritePosition = spritePos_->GetPosition();

		//ビューポート行列
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, 1280, 720, 0, 1);
		//ビュープロジェクションビューポート合成行列
		Matrix4x4 matVPV = Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

		//合成行列の逆行列を計算する
		Matrix4x4 matInverseVPV = Inverse(matVPV);

		//スクリーン座標
		Vector3 posNear = Vector3(spritePosition.x, spritePosition.y, 0);
		Vector3 posFar = Vector3(spritePosition.x, spritePosition.y, 1);

		//スクリーン座標系からワールド座標系へ
		posNear = TransformNormal(posNear, matInverseVPV);
		posFar = TransformNormal(posFar, matInverseVPV);

		//マウスレイの方向
		Vector3 mouseDirection = Subtract(posFar, posNear);
		mouseDirection = Normalize(mouseDirection);

		//カメラから照準オブジェクトの距離
		const float kDistanceTestObject = 50.0f;
		worldTransformReticle_.translation_ = Multiply(kDistanceTestObject, mouseDirection);

		//キャラクターの座標を画面表示する処理
#ifdef _DEBUG

		/*ImGui::Begin("window");
		if (ImGui::TreeNode("Reticle")) {
			ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
			ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
			ImGui::Text("2DReticle:(%f,%f)", spritePos_->GetPosition().x, spritePos_->GetPosition().y);
			ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransformReticle_.translation_.x,
				worldTransformReticle_.translation_.y, worldTransformReticle_.translation_.z);
			ImGui::TreePop();
		}
		ImGui::End();*/

#endif // _DEBUG

		//worldTransform3DReticle_のワールド行列更新と転送
		worldTransformReticle_.UpdateMatrix();
	}
}
void Player::OnCollision()
{
	isDead_ = true;
}

void Player::Reset()
{
	isDead_ = false;
	isAttack = false;

	velocity_ = { 0.0f, 0.0f, 0.0f };

	//ワールド変換の初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translation_.y = 3.0f;
	//体
	worldTransformBody_.Initialize();
	worldTransformBody_.parent_ = &worldTransformBase_;
	//頭
	worldTransformFace_.Initialize();
	worldTransformFace_.parent_ = &worldTransformBody_;
	//左腕
	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	//右腕
	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	//浮遊ギミック初期化
	InitializeFloatingGimmick();
}
