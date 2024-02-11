#include "Player.h"
#include "newMath.h"

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::InitializeFloatingGimmick() { 
	floatingParameter_ = 0.0f;
}

void Player::UpdateFloatingGimmick() { 
	//浮遊移動のサイクル
	const uint16_t period = 60;

	//1フレームでのパラメーター加算値
	const float step = 2.0f * M_PI / period;

	//パラメータを1ステップ分加算
	floatingParameter_ += step;

	//2πを越えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * M_PI);

	//浮遊の振幅＜ｍ＞
	const float SW = 2;

	//浮遊を座標に反映
	worldTransform_.translation_.y = std::sin(floatingParameter_) * SW;


}

void Player::Attack() {
	if (input_->PushKey(DIK_SPACE)) {
		if (count == 0) {
			// 弾の速度
			const float kBulletSpeed = 1.0f;
			Vector3 velocity(0, 0, kBulletSpeed);
			velocity = Math::TransformNormal(velocity, worldTransform_.matWorld_);

			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(model_, worldTransform_.translation_, velocity);
			// 弾を登録
			// bullet_ = newBullet;
			bullets_.push_back(newBullet);
			count++;
		}
	} else {
		count = 0;
	}
}

void Player::Initialize(Model* model, uint32_t textureHandle) {

	// NULLポインタチェック
	assert(model);
	// 体
	model_ = model;
	// 頭
	modelHead_ = model;

	// 手
	modelL_ = model;
	modelR_ = model;
	// 足
	modelLL_ = model;
	modelLR_ = model;

	// 武器
	modelWeapon_ = model;

	textureHandle_ = textureHandle;

	// 体のワールドトランスフォーム
	worldTransform_.Initialize();
	// 頭のワールドトランスフォーム
	worldTransformHead_.Initialize();
	// 手のワールドトランスフォーム
	worldTransformL_.Initialize();
	worldTransformR_.Initialize();
	// 足のワールドトランスフォーム
	worldTransformLL_.Initialize();
	worldTransformLR_.Initialize();

	// 付け根左
	worldTransformRootL_.Initialize();

	// 付け根右
	worldTransformRootR_.Initialize();

	// 武器
	worldTransformWeaPon_.Initialize();

	// 親子関係
	// 頭
	worldTransformHead_.parent_ = &worldTransform_;
	// 左手
	worldTransformL_.parent_ = &worldTransform_;
	// 右手
	worldTransformR_.parent_ = &worldTransform_;
	// 左足
	worldTransformLL_.parent_ = &worldTransformRootL_;
	// 右足
	worldTransformLR_.parent_ = &worldTransformRootR_;

	// 付け根左
	worldTransformRootL_.parent_ = &worldTransform_;

	// 付け根右
	worldTransformRootR_.parent_ = &worldTransform_;

	//武器
	worldTransformWeaPon_.parent_ = &worldTransformR_;

	/***********
	   位置
	***********/
	//  体
	worldTransform_.translation_.z = 50.0f;
	//	頭
	worldTransformHead_.translation_.x = 0.0;
	worldTransformHead_.translation_.y = 3.0f;
	// 右手
	worldTransformR_.translation_.x = 2.0f;
	worldTransformR_.translation_.y = 2.0f;
	// 左手
	worldTransformL_.translation_.x = -2.0f;
	worldTransformL_.translation_.y = 2.0f;
	////右足
	worldTransformLR_.translation_.x = 0.0f;
	worldTransformLR_.translation_.y = -4.0f;
	////左足
	worldTransformLL_.translation_.x = 0.0f;
	worldTransformLL_.translation_.y = -4.0f;

	worldTransformRootL_.translation_.x = -2.0f;

	worldTransformRootR_.translation_.x = 2.0f;

	worldTransformWeaPon_.translation_.x = 3.0f;
	worldTransformWeaPon_.translation_.y = 3.0f;

	worldTransformWeaPon_.scale_.y = 5.0f;

	input_ = Input::GetInstance();
	InitializeFloatingGimmick();
};

void Player::OnCollision() {}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.x = worldTransform_.matWorld_.m[3][1];
	worldPos.x = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::Update() {

	// デスフラグの立った球を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	worldTransform_.UpdateMatrix();

	worldTransformHead_.UpdateMatrix();

	worldTransformL_.UpdateMatrix();

	worldTransformR_.UpdateMatrix();

	worldTransformLL_.UpdateMatrix();

	worldTransformLR_.UpdateMatrix();

	worldTransformRootL_.UpdateMatrix();

	worldTransformRootR_.UpdateMatrix();

	worldTransformWeaPon_.UpdateMatrix();

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
		inputFloat3[0] = worldTransform_.translation_.x;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
		inputFloat3[0] = worldTransform_.translation_.x;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
		inputFloat3[1] = worldTransform_.translation_.y;
	} else if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
		inputFloat3[1] = worldTransform_.translation_.y;
	}

	const float kRotSpeed = 0.02f;

	// 押した方向でベクトル移動
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}

	// 左足の動き
	//worldTransformRootL_.rotation_.x += 0.01f;
	//if (worldTransformRootL_.rotation_.x >= 1.0f) {
		//worldTransformRootL_.rotation_.x -= 0.01f;
//	}

	//
	worldTransformRootR_.rotation_.x -= 0.00f;

	// ImGui加算用
	// worldTransform_.translation_.x = inputFloat3[0];
	// worldTransform_.translation_.y = inputFloat3[1];

	// ベクターの加算
	worldTransform_.translation_ = Math::Add(worldTransform_.translation_, move);
	// アフィン変換行列の作成
	worldTransform_.matWorld_ = Math::MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);


	UpdateFloatingGimmick();


	// ImGuiスライダー
#ifdef _DEBUG

	ImGui::Begin("PlayerDebug");
	if (ImGui::TreeNode(" playerBody")) {
		ImGui::Text("DebugCamera Toggle : ENTER");
		ImGui::SliderFloat3("Positions", &worldTransform_.translation_.x, -20.0f, 20.0f);
		ImGui::SliderFloat3("Rotation", &worldTransform_.rotation_.x, -20.0f, 20.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" playerHead")) {
		ImGui::Text("DebugCamera Toggle : ENTER");
		ImGui::SliderFloat3("Positions", &worldTransformHead_.translation_.x, -20.0f, 20.0f);
		ImGui::SliderFloat3("Rotation", &worldTransformHead_.rotation_.x, -20.0f, 20.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" playerLeft")) {
		ImGui::Text("DebugCamera Toggle : ENTER");
		ImGui::SliderFloat3("Positions", &worldTransformL_.translation_.x, -20.0f, 20.0f);
		ImGui::SliderFloat3("Rotation", &worldTransformL_.rotation_.x, -20.0f, 20.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" playerRight")) {
		ImGui::Text("DebugCamera Toggle : ENTER");
		ImGui::SliderFloat3("Positions", &worldTransformR_.translation_.x, -20.0f, 20.0f);
		ImGui::SliderFloat3("Rotation", &worldTransformR_.rotation_.x, -20.0f, 20.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" playerLeftASI")) {
		ImGui::Text("DebugCamera Toggle : ENTER");
		ImGui::SliderFloat3("Positions", &worldTransformLL_.translation_.x, -20.0f, 20.0f);
		ImGui::SliderFloat3("Rotation", &worldTransformLL_.rotation_.x, -20.0f, 20.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" playerRightASI")) {
		ImGui::Text("DebugCamera Toggle : ENTER");
		ImGui::SliderFloat3("Positions", &worldTransformLR_.translation_.x, -20.0f, 20.0f);
		ImGui::SliderFloat3("Rotation", &worldTransformLR_.rotation_.x, -20.0f, 20.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("BUKI")) {
		ImGui::Text("DebugCamera Toggle : ENTER");
		ImGui::SliderFloat3("Positions", &worldTransformWeaPon_.translation_.x, -20.0f, 20.0f);
		ImGui::SliderFloat3("Rotation", &worldTransformWeaPon_.rotation_.x, -20.0f, 20.0f);

		ImGui::TreePop();
	}

	// ImGui終わり
	ImGui::End();
#endif // DEBUG

	// 移動限界座標
	const float kMoveLimitX = 34;
	const float kMoveLimitY = 18;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	Attack();
	worldTransform_.UpdateMatrix();
	/* if (bullet_) {
	    bullet_->Update();
	}*/

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
};

void Player::Draw(ViewProjection viewProjection) {

	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	model_->Draw(worldTransformHead_, viewProjection, textureHandle_);

	modelL_->Draw(worldTransformL_, viewProjection, textureHandle_);

	modelR_->Draw(worldTransformR_, viewProjection, textureHandle_);

	modelLL_->Draw(worldTransformLL_, viewProjection, textureHandle_);

	modelLR_->Draw(worldTransformLR_, viewProjection, textureHandle_);

	modelWeapon_->Draw(worldTransformWeaPon_, viewProjection, textureHandle_);

	// 弾描画
	/* if (bullet_) {
	    bullet_->Draw(viewProjection);
	}*/

	// 弾の描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
};

void Player::SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }