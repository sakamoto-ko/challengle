#include "Player.h"

#include "MyMath.h"

#include <cassert>
#include <list>

#include "GameScene.h"

void Player::OnCollision() {
	tex_ = textures_[2];
	hp_ -= 20;
	//isHit = true;

	if (hp_ <= 0) {
		//デスフラグを立てる
		//isDead_ = true;
	}
}

Player::Player() {}
Player::~Player() {}

Vector3 Player::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos = {};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::Save() {
	worldTransformTemp_ = worldTransform_;
	worldTransformTemp_.UpdateMatrix();
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
	bulletTex_ = textures_[0];

	//スプライト
	spritePos_.reset(Sprite::Create(textures_[3], { 0.0f, 0.0f }));
	spritePos_->SetSize({ 96.0f, 96.0f });
	spritePos_->SetTextureRect({ 0.0f,0.0f, }, { 1500.0f, 1500.0f });
	spritePos_->SetPosition({ 0.0f, 0.0f });

	worldTransformTemp_.Initialize();
	worldTransformTemp_.translation_ = { 0.0f,0.0f,4.0f };
	worldTransformTemp_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransformTemp_.scale_ = { 0.2f,0.2f,0.2f };
	worldTransformTemp_.UpdateMatrix();

	worldTransformReticle_.Initialize();
	Reset();
}

// 更新
void Player::Update(const ViewProjection viewProjection) {

	assert(gameScene_);

	if (!isHit) {
		tex_ = textures_[0];
	}

	//キー入力の更新
	button->Update();

	//Input();

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
		case Player::Behavior::kDive:
			BehaviorDiveInitialize();
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
		//ジャンプ行動
	case Behavior::kDive:
		BehaviorDiveUpdate();
		break;
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("player")) {
		if (ImGui::TreeNode("base")) {
			ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
			ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.01f);
			ImGui::Text("hp %d", hp_);
			ImGui::Text("bullet %d", haveBullet);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("behavior")) {
			ImGui::Text("behavior %d", behavior_);
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
	if (!isDead_) {
		models_[0]->Draw(worldTransform_, viewProjection, tex_);
	}
}

//UI描画
void Player::DrawUI() {
	//ロックオンしていれば
	if (spritePos_) {
		if (spritePos_->GetPosition().x != 0.0f &&
			spritePos_->GetPosition().y != 0.0f) {
			spritePos_->Draw();
		}
	}
}

//通常行動初期化
void Player::BehaviorRootInitialize() {}
//通常行動更新
void Player::BehaviorRootUpdate() {

	GetReticlePosition(*viewProjection_);

	if (isHit) {
		if (++hitCount >= 60) {
			isHit = false;
		}
	}

	if (isMove) {
		Move();
		if (++moveFinishTime >= 1) {
			isMove = false;
			velocity_.x = 0.0f;
			moveFinishTime = 0;
		}
	}
	if (isJump) {
		PlusGravity();
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

	//worldTransform_.translation_.y = 0.0f;
}
//ジャンプ行動更新
void Player::BehaviorJumpUpdate() {

	if (velocity_.y <= 0.0f) {
		velocity_.y = 0.0f;
		BehaviorSetRoot();
	}
	else {
		//移動
		velocity_.y += accelerationVector.y;
		worldTransform_.translation_.y += velocity_.y;
		isJump = true;
	}

	Landing();
	worldTransform_.UpdateMatrix();
}

void Player::BehaviorDiveInitialize() {
	velocity_.y = 0.0f;
	isDive = true;
}
void Player::BehaviorDiveUpdate() {
	//移動
	velocity_.y += accelerationVector.y;
	worldTransform_.translation_.y += velocity_.y;

	Landing();
	worldTransform_.UpdateMatrix();
}

void Player::BehaviorAttackInitialize() {}
void Player::BehaviorAttackUpdate() {
	if (spritePos_ != nullptr) {
		Shot();
	}
}

void Player::Move() {
	//移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	worldTransform_.UpdateMatrix();
}

void Player::Shot() {
	assert(gameScene_);

	//弾を所持していれば
	if (haveBullet > 0) {
		//弾の速度
		const float kBulletSpeed = 0.1f;

		Vector3 bulletVelocity = {};

		//自機から照準オブジェクトへのベクトル
		bulletVelocity = Subtract(worldTransformReticle_.translation_, worldTransform_.translation_);
		bulletVelocity = Multiply(kBulletSpeed, Normalize(bulletVelocity));

		//弾を生成し、初期化
		playerBulletNum_++;
		haveBullet--;
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(
			bulletModels_, bulletTextures_,
			{ worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1],worldTransform_.matWorld_.m[3][2] }
		, bulletVelocity);

		//弾をゲームシーンに登録する
		gameScene_->AddPlayerBullet(newBullet);
	}

	isAttack = false;
	BehaviorSetRoot();
}

void Player::Input() {
	//キー入力の更新
	button->Update();

	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if (!isMove) {
			//右を押したら
			if (button->isTriggerRight()) {
				velocity_.x = 0.5f;
				isMove = true;
			}
			//左を押したら
			else if (button->isTriggerLeft()) {
				velocity_.x = -0.5f;
				isMove = true;
			}
			else {
				velocity_.x = 0.0f;
			}
		}

		//上を押したら
		if (button->isTriggerUp()) {
			BehaviorSetJump();
		}
		//下を押したら
		if (button->isTriggerDown()) {
			BehaviorSetDive();
		}

		if (!isAttack) {
			//Rトリガーを押していたら
			if (button->isTriggerAttack()) {
				if (playerBulletNum_ < playerBulletMax_) {
					isAttack = true;
					BehaviorSetAttack();
				}
			}
		}
	}
}

void Player::Reset()
{
	tex_ = textures_[0];
	isDead_ = false;
	isAttack = false;
	isJump = false;
	isDive = false;
	isMove = false;
	isHit = false;

	haveBullet = 10;

	worldTransform_.Initialize();
	worldTransform_ = worldTransformTemp_;
	worldTransform_.UpdateMatrix();

	velocity_ = { 0.0f,0.0f,0.0f };
	BehaviorSetRoot();
}

void Player::Landing() {
	//着地
	if (worldTransform_.translation_.y <= 0.0f) {
		worldTransform_.translation_.y = 0.0f;
		//ジャンプ終了
		isJump = false;
		isDive = false;
		velocity_.y = 0.0f;
		worldTransform_.translation_.y = 0.0f;
		BehaviorSetRoot();
	}
	worldTransform_.UpdateMatrix();
}

void Player::PlusGravity() {
	Vector3 velocity = { 0.0f, -0.01f,0.0f };
	worldTransform_.translation_.y += velocity.y;

	Landing();
	worldTransform_.UpdateMatrix();
}

void Player::BehaviorSetRoot() {
	behaviorRequest_ = Behavior::kRoot;
}
void Player::BehaviorSetRight() {
	//behaviorRequest_ = Behavior::kRight;
}
void Player::BehaviorSetLeft() {
	//behaviorRequest_ = Behavior::kLeft;
}
void Player::BehaviorSetJump() {
	behaviorRequest_ = Behavior::kJump;
}
void Player::BehaviorSetDive() {
	behaviorRequest_ = Behavior::kDive;
}
void Player::BehaviorSetAttack() {
	behaviorRequest_ = Behavior::kAttack;
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

		ImGui::Begin("window");
		if (ImGui::TreeNode("Reticle")) {
			ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
			ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
			ImGui::Text("2DReticle:(%f,%f)", spritePos_->GetPosition().x, spritePos_->GetPosition().y);
			ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransformReticle_.translation_.x,
				worldTransformReticle_.translation_.y, worldTransformReticle_.translation_.z);
			ImGui::TreePop();
		}
		ImGui::End();

#endif // _DEBUG

		//worldTransform3DReticle_のワールド行列更新と転送
		worldTransformReticle_.UpdateMatrix();
	}
}