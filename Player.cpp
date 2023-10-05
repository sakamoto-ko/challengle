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
	if (ImGui::TreeNode("stick")) {
		ImGui::Text("left %f", (float)joyState.Gamepad.sThumbLY / SHRT_MAX);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG

}

// 初期化
void Player::Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures) {
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	SetModels(models);
	SetTextures(textures);

	tex_ = textures_[0];

	worldTransform_.Initialize();

	worldTransform_.translation_ = { -18.0f,-0.47f,40.0f };
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.scale_ = { 2.5f,2.5f,1.0f };

	worldTransform_.UpdateMatrix();

	hp = 100;
	isMove_ = false;
	moveCount_ = 0;
}

// 更新
void Player::Update(const ViewProjection viewProjection) {

	//死ぬ
	if (hp <= 0) {
		isDead_ = true;
	}

	Move();

	worldTransform_.UpdateMatrix();

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("player")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
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