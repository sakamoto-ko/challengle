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

void Player::OnCollision() {
	hp -= 20;
}

//親となるトランスフォームをセット
void Player::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

void Player::Move() {
	XINPUT_STATE joyState;

	if (input_->GetJoystickState(0, joyState)) {
		if (!isMove_) {
			if ((float)joyState.Gamepad.sThumbLY / SHRT_MAX >= 0.5f) {
				lane_++;

				isMove_ = true;;
			}
			else if ((float)joyState.Gamepad.sThumbLY / SHRT_MAX <= -0.5f) {
				lane_--;

				isMove_ = true;;
			}
			else {
				lane_ = lane_;
			}
		}
		else {
			if (++moveCount_ >= 30) {
				isMove_ = false;
				moveCount_ = 0;
			}
		}

		if (lane_ > TOP) {
			lane_ = TOP;
		}
		else if (lane_ < BOTTOM) {
			lane_ = BOTTOM;
		}
	}

	switch (lane_) {
	case TOP:
		worldTransform_.translation_ = { -18.0f,9.37f,40.0f };
		break;
	default:
	case MID:
		worldTransform_.translation_ = { -18.0f,-0.47f,40.0f };
		break;
	case BOTTOM:
		worldTransform_.translation_ = { -18.0f,-9.77f,40.0f };
		break;
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("device")) {
		if (ImGui::TreeNode("button")) {
			ImGui::Text("Press %d", button->isPress());
			ImGui::Text("Trigger %d", button->isTrigger());
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("stick")) {
			ImGui::Text("%f", (float)joyState.Gamepad.sThumbLY / SHRT_MAX);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}

void Player::Attack() {
	if (!isAttack_) {
		if (button->isPress()) {
			if (++attackChargeCount_ >= 40) {
				isAttack_ = true;
			}
		}
		else {
			attackChargeCount_ = 0;
		}
	}
	else {
		Shot();
		if (++attackCoolDown_ >= 60) {
			isAttack_ = false;
			attackCoolDown_ = 0;
			attackChargeCount_ = 0;
			playerBulletNum_ = 0;
		}
	}
}

void Player::Shot() {
	assert(gameScene_);

	//弾の速度
	const float kBulletSpeed = 1.0f;
	Vector3 velocity(kBulletSpeed, 0, 0);

	//自機から照準オブジェクトへのベクトル
	velocity.x = 2.0f;
	velocity.y = 0.0f;
	velocity.z = 0.0f;
	velocity = Multiply(kBulletSpeed, Normalize(velocity));

	//弾を生成し、初期化
	if (playerBulletNum_ < PLAYER_BULLET_MAX) {
		playerBulletNum_++;
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(
			bulletModels_, bulletTextures_,
			{ worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1],worldTransform_.matWorld_.m[3][2] }
		, velocity);

		//弾をゲームシーンに登録する
		gameScene_->AddPlayerBullet(newBullet);
	}
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

	worldTransform_.Initialize();

	worldTransform_.translation_ = { -18.0f,-0.47f,32.5f };
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.scale_ = { 2.5f,2.5f,2.5f };

	worldTransform_.UpdateMatrix();

	hp = 100;
	isMove_ = false;
	moveCount_ = 0;
}

// 更新
void Player::Update(const ViewProjection viewProjection) {

	//キー入力の更新
	button->Update();

	//移動処理
	Move();

	//攻撃
	Attack();

	worldTransform_.UpdateMatrix();

	//死亡処理
	if (hp <= 0) {
		isDead_ = true;
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("player")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
		ImGui::Text("hp %d", hp);
		ImGui::Text("isAttack %d", isAttack_);
		ImGui::Text("charge %d", attackChargeCount_);
		ImGui::Text("cooldown %d", 60 - attackCoolDown_);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}

// 描画
void Player::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransform_, viewProjection, tex_);
}

//UI描画
void Player::DrawUI() {
}