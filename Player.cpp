#include "Player.h"

#include "MyMath.h"

#include <cassert>
#include <list>

#include "GameScene.h"

Player::Player() {

}
Player::~Player() {
	delete sprite2DReticle_;
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

//旋回
void Player::Rotate() {
	//回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}
	else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Shot() {
	assert(gameScene_);

	//弾の速度
	const float kBulletSpeed = 1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	//速度ベクトルを自機の向きに合わせて回転させる
	//velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	//自機から照準オブジェクトへのベクトル
	velocity.x = worldTransform3DReticle_.translation_.x - worldTransform_.translation_.x;
	velocity.y = 2.0f;
	velocity.z = worldTransform3DReticle_.translation_.z - worldTransform_.translation_.z;
	velocity = Multiply(kBulletSpeed, Normalize(velocity));

	//弾を生成し、初期化
	PlayerBullet* newBullet = new PlayerBullet();
	newBullet->Initialize(
		modelBullet_, textureHandleBullet_,
		{ worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1],worldTransform_.matWorld_.m[3][2] }
	, velocity);

	//弾をゲームシーンに登録する
	gameScene_->AddPlayerBullet(newBullet);
}

//攻撃
void Player::Attack() {
	assert(gameScene_);

	XINPUT_STATE joyState;

	//ゲームパッド未接続なら何もせずに抜ける
	/*if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}*/

	//Rトリガーを押していたら
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			Shot();
		}
	}
	else if (input_->PushKey(DIK_SPACE) ||
		input_->IsPressMouse(0)) {
		Shot();
	}
}

void Player::OnCollision() {
	hp -= 20;
}

//親となるトランスフォームをセット
void Player::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
	worldTransform3DReticle_.parent_ = parent;
}

//自機のワールド座標から3Dレティクルのワールド座標を計算
void Player::Get3DReticleWorldPosition() {
	//徐壱岐から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	//自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = { 0,0,1.0f };
	//自機のワールド行列の回転を反映
	Matrix4x4 rot = MakeRotateMatrix(worldTransform_.rotation_);
	offset = Multiply(offset, rot);
	//ベクトルの長さを整える
	offset = Multiply(kDistancePlayerTo3DReticle, Normalize(offset));
	//3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ = Add(offset, worldTransform_.translation_);
	//WorldTransformの更新と転送
	worldTransform3DReticle_.UpdateMatrix();
}

//3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
//void Player::Convert3Dto2DCoord(const ViewProjection viewProjection) {
//	Vector3 spritePosition = worldTransform3DReticle_.translation_;
//	//ビューポート行列
//	Matrix4x4 matViewport = MakeViewportMatrix(0, 0,
//		WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
//	//ビュー行列とプロジェクション行列、ビューポート行列を合成する
//	Matrix4x4 matViewProjectionViewport =
//		Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
//	//ワールド→スクリーン座標変換(ここで3Dから2Dになる)
//	spritePosition = TransformNormal(spritePosition, matViewProjectionViewport);
//	//スプライトのレティクルに座標設定
//	sprite2DReticle_->SetPosition(Vector2(spritePosition.x, spritePosition.y));
//}

//マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
void Player::GetMouseWorldPosition(const ViewProjection viewProjection) {
	POINT mousePosition;
	//マウス座標(スクリーン座標)を取得する
	GetCursorPos(&mousePosition);

	//スプライトの現在座標を取得
	Vector2 spritePosition = sprite2DReticle_->GetPosition();
	//XINPUT_STATE joyState;

	//クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	spritePosition = { 640.0f,360.0f };
	sprite2DReticle_->SetPosition(spritePosition);

	//ジョイスティック状態取得
	//if (Input::GetInstance()->GetJoystickState(0, joyState)) {
	//	/*spritePosition.x += ((float)joyState.Gamepad.sThumbRX / SHRT_MAX) * 10.0f;
	//	spritePosition.y -= ((float)joyState.Gamepad.sThumbRY / SHRT_MAX) * 10.0f;*/
	//	spritePosition = { 640.0f,360.0f };
	//	//スプライトの座標変更を反映
	//	sprite2DReticle_->SetPosition(spritePosition);
	//}
	//else {
	//	//マウス座標を2Dレティクルのスプライトに代入する
	//	spritePosition = { (float)mousePosition.x, (float)mousePosition.y };
	//	//spritePosition = { 640.0f,360.0f };
	//	sprite2DReticle_->SetPosition(spritePosition);
	//}

	//ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
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
	worldTransform3DReticle_.translation_ = Add(posNear, Multiply(kDistanceTestObject, mouseDirection));
	//spritePosition.x = Multiply(kDistanceTestObject, mouseDirection).x;
	//spritePosition.y = Multiply(kDistanceTestObject, mouseDirection).y;

	//worldTransform3DReticle_のワールド行列更新と転送
	worldTransform3DReticle_.UpdateMatrix();

#ifdef _DEBUG

	//キャラクターの座標を画面表示する処理
	ImGui::Begin("Player");
	ImGui::DragFloat3("Player.translation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Player.rotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::Text("2DReticle:(%f,%f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
	ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
	ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
	ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
		worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	ImGui::End();
#endif // _DEBUG

}

// 初期化
void Player::Initialize(Model* model, Model* modelBullet, uint32_t textureHandle, uint32_t textureHandleBullet, uint32_t textureHandleReticle) {
	assert(model);

	model_ = model;
	modelBullet_ = modelBullet;
	textureHandle_ = textureHandle;
	textureHandleBullet_ = textureHandleBullet;
	textureHandleReticle_ = textureHandleReticle;

	//レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("re.png");

	//スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, { worldTransform_.translation_.x,worldTransform_.translation_.y }, Vector4{ 1,1,1,1 }, Vector2{ 0.5f,0.5f });

	worldTransform_.Initialize();

	worldTransform_.translation_.y = 2.0f;

	//3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	move = { 0.0f,0.0f,0.0f };

	hp = 100;

	worldTransform_.UpdateMatrix();

}

// 更新
void Player::Update(const ViewProjection viewProjection) {
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

	//死ぬ
	if (hp <= 0) {
		isDead_ = true;
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
	//マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	GetMouseWorldPosition(viewProjection);
}

// 描画
void Player::Draw(ViewProjection viewProjection) {
	//model_->Draw(worldTransform3DReticle_, viewProjection, textureHandleReticle_);
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

//UI描画
void Player::DrawUI() {
	sprite2DReticle_->Draw();
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
	Shot();
}

//攻撃行動初期化
void Player::BehaviorAttackInitialize() {
	afterAttackStay = 20;
	Shot();
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
	//worldTransform_.translation_.y = std::sin(idelArmAngleMax_) * floatingAmplitude_ + 5.0f;
	//worldTransformL_arm_.rotation_.x = std::sin(idelArmAngleMax_) * floatingAmplitude_;
	//worldTransformR_arm_.rotation_.x = -std::sin(idelArmAngleMax_) * floatingAmplitude_;

#ifdef _DEBUG

	ImGui::Begin("Player");
	/*ImGui::SliderFloat3("face.translation", &worldTransformFace_.translation_.x, -10, 10);
	ImGui::SliderFloat3("body.translation", &worldTransformBody_.translation_.x, -10, 10);
	ImGui::SliderFloat3("L_arm.translation", &worldTransformL_arm_.translation_.x, -10, 10);
	ImGui::SliderFloat3("R_arm.translation", &worldTransformR_arm_.translation_.x, -10, 10);
	ImGui::SliderFloat3("weapon.translation_", &worldTransformWeapon_.translation_.x, -10, 10);
	ImGui::SliderFloat3("weapon.rotation_", &worldTransformWeapon_.rotation_.x, -10, 10);*/
	ImGui::SliderInt("period", reinterpret_cast<int*>(&floatingCycle_), 1, 144);
	ImGui::SliderFloat("amplitude", &floatingAmplitude_, 0.0f, PAI * 2.0f);
	ImGui::End();
#endif // _DEBUG

}

//通常行動更新
void Player::BehaviorRootUpdate() {
	assert(gameScene_);
	// キャラクターの移動ベクトル
	//Vector3 move = { 0, 0, 0 };

	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// キャラクターの移動速さ
	//const float kChatacterSpeed = 0.2f;

	////ジョイスティック状態取得
	//if (Input::GetInstance()->GetJoystickState(0, joyState)) {

	//}
	//else {
	//	// 押した方向で移動ベクトルを変更(左右)
	//	if (input_->PushKey(DIK_LEFT)) {
	//		move.x -= kChatacterSpeed;
	//	}
	//	else if (input_->PushKey(DIK_RIGHT)) {
	//		move.x += kChatacterSpeed;
	//	}

	//	// 押した方向で移動ベクトルを変更(上下)
	//	if (input_->PushKey(DIK_UP)) {
	//		move.y += kChatacterSpeed;
	//	}
	//	else if (input_->PushKey(DIK_DOWN)) {
	//		move.y -= kChatacterSpeed;
	//	}
	//}

	//// 旋回処理
	//Rotate();

	////座標移動(ベクトルの加算)
	//worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	////移動限界座標
	//const float kMoveLimitX = 30;
	//const float kMoveLimitY = 18;

	////範囲を超えない処理
	//worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	//worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	//worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	//worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	////WorldTransformの更新
	//worldTransform_.UpdateMatrix();


	//Rトリガーを押していたら
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//Rトリガーを押していたら
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			Shot();
		}
		/*else if (input_->PushKey(DIK_SPACE) ||
			input_->IsPressMouse(0)) {
			Shot();
		}*/

		//浮遊ギミック更新
		UpdateFloatingGimmick();

		//速さ
		const float speed = 0.4f;
		//移動量
		move.x = (float)joyState.Gamepad.sThumbLX / SHRT_MAX * speed;
		move.z = (float)joyState.Gamepad.sThumbLY / SHRT_MAX * speed;
		/*Vector3 move = {
			(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
			0,
			(float)joyState.Gamepad.sThumbLY / SHRT_MAX
		};*/
		//移動量に速さを反映
		move = Multiply(speed, Normalize(move));

		//移動ベクトルをカメラの角度だけ回転する
		Matrix4x4 rotate = MakeRotateMatrix(viewProjection_->rotation_);
		move = TransformNormal(move, rotate);

		//移動方向に向きを合わせる
		//Y軸周り角度(θy)
		worldTransform_.rotation_.y = std::atan2(-move.x, -move.z);

		//移動
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	}

	//行動制限
	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, -50.0f, 50.0f);
	worldTransform_.translation_.z = Clamp(worldTransform_.translation_.z, -50.0f, 50.0f);

	worldTransform_.UpdateMatrix();

}

//攻撃行動更新
void Player::BehaviorAttackUpdate() {
	assert(gameScene_);

	worldTransform_.UpdateMatrix();

	behaviorRequest_ = Behavior::kRoot;
}
