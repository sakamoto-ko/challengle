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

// 初期化
void Player::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// キー入力 初期化
	button->Initialize();

	SetModels(models);
	SetTextures(textures);
	bulletTextures_ = textures;
	bulletModels_ = models;
	tex_ = textures_[0];
	bulletTex_ = textures_[0];

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

	//XINPUT_STATE joyState;
	tex_ = textures_[0];

	//キー入力の更新
	button->Update();

	if (behaviorRequest_) {
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞い事の初期化を実行
		switch (behavior_) {
		case Player::Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		/*case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
			break;*/
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
	/*case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;*/
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
}
//通常行動更新
void Player::BehaviorRootUpdate() {

	if(isMove){
		Move();
		if (++moveFinishTime >= 3) {
			isMove = false;
			velocity_.x = 0.0f;
			moveFinishTime = 0;
		}
	}
	if (isJump) {
		behaviorRequest_ = Behavior::kJump;
	}
	if (isAttack) {
		Shot();
	}
	if (playerBulletNum_ >= 1) {
		if (++shotFinishTime >= 30) {
			playerBulletNum_ = 0;
			shotFinishTime = 0;
		}
	}

	worldTransform_.UpdateMatrix();
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

void Player::Move() {
	//移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	worldTransform_.UpdateMatrix();
}

void Player::Shot() {
	assert(gameScene_);

	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	//弾の速度
	const float kBulletSpeed = 0.06f;

	Vector3 bulletVelocity = { 0.0f, 0.0f ,0.0f };

	//自機から照準オブジェクトへのベクトル
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		bulletVelocity.x = (float)joyState.Gamepad.sThumbRX / SHRT_MAX;
		bulletVelocity.y = (float)joyState.Gamepad.sThumbRY / SHRT_MAX;
		bulletVelocity.z = 0.0f;
	}

	bulletVelocity = Multiply(kBulletSpeed, Normalize(bulletVelocity));

	//弾を生成し、初期化
	if (bulletVelocity.x != 0.0f || bulletVelocity.y != 0.0f) {
		playerBulletNum_++;
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(
			bulletModels_, bulletTextures_,
			{ worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1],worldTransform_.matWorld_.m[3][2] }
		, bulletVelocity);

		//弾をゲームシーンに登録する
		gameScene_->AddPlayerBullet(newBullet);
	}

	isAttack = false;
	behaviorRequest_ = Behavior::kRoot;
}

void Player::Input() {
	//キー入力の更新
	button->Update();

	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//右を押していたら
		if (!isMove) {
			if (button->isTriggerRight()) {
				velocity_.x = 1.0f * 0.1f;
				isMove = true;
			}
			//左を押していたら
			else if (button->isTriggerLeft()) {
				velocity_.x = -1.0f * 0.1f;
				isMove = true;
			}
			else {
				velocity_.x = 0.0f;
			}
		}

		//Rトリガーを押していたら
		if (button->isTriggerAttack()) {
			if (playerBulletNum_ < playerBulletMax_) {
				if (!isAttack) {
					isAttack = true;
				}
			}
		}
		//ジャンプボタンを押したら
		if (button->isTriggerJump()) {
			if (!isJump) {
				isJump = true;
			}
		}
	}

	tex_ = textures_[1];
}

void Player::Reset()
{
	tex_ = textures_[0];
	isDead_ = false;

	worldTransform_.Initialize();

	worldTransform_.translation_ = { 0.0f,0.0f,4.0f };
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.scale_ = { 0.0f,0.0f,0.0f };

	worldTransform_.UpdateMatrix();

	velocity_ = { 0.0f,0.0f,0.0f };
}