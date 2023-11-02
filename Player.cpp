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
	const float speed = 0.3f;
	//移動量
	velocity_ = {
		(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
		(float)joyState.Gamepad.sThumbLY / SHRT_MAX,
		0.0f
	};

	//移動量に速さを反映
	velocity_ = Multiply(speed, Normalize(velocity_));

	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
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
