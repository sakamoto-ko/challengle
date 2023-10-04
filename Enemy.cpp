#include "Enemy.h"
#include "Player.h"
#include "GameScene.h"

#include "MyMath.h"

#include <cassert>
#include <list>

//staticで宣言したメンバ関数ポインタテーブルの実体
void (Enemy::* Enemy::spFuncTable[])() = {
	&Enemy::ApproachUpdate,
	&Enemy::LeaveUpdate,
};

void Enemy::ApproachInit() {
	//発射タイマーを初期化
	shotTimer = kFireInterval;
}

void Enemy::ApproachUpdate() {
	assert(player_);

	//移動(ベクトルを加算)
	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 worldPos = Subtract(playerPos, worldTransform_.translation_);
	Vector3 velocity = { Normalize(worldPos) };

	//速さ
	const float speed = 0.2f;
	//移動量に速さを反映
	velocity = Multiply(speed, Normalize(velocity));

	//移動ベクトルをカメラの角度だけ回転する
	Matrix4x4 rotate = MakeRotateMatrix(Multiply(1.0f, viewProjection_->rotation_));
	velocity = TransformNormal(velocity, rotate);

	worldTransform_.translation_.x += velocity.x;
	worldTransform_.translation_.z += velocity.z;

	//移動方向に向きを合わせる
	//Y軸周り角度(θy)
	worldTransform_.rotation_.y = std::atan2(-velocity.x, -velocity.z);

	//行動制限
	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x, -50.0f, 50.0f);
	worldTransform_.translation_.z = Clamp(worldTransform_.translation_.z, -50.0f, 50.0f);

	//WorldTransformの更新
	worldTransform_.UpdateMatrix();

	//基底の位置に到達したら離脱
	if (sqrtf(worldPos.x * worldPos.x) <= 10.0f &&
		sqrtf(worldPos.z * worldPos.z) <= 10.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeaveUpdate() {
	//移動(ベクトルを加算)
	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 worldPos = Subtract(playerPos, worldTransform_.translation_);

	//既定の位置に到達したら接近
	if (sqrtf(worldPos.x * worldPos.x) >= 10.0f ||
		sqrtf(worldPos.z * worldPos.z) >= 10.0f) {
		phase_ = Phase::Approach;
	}

	//発射タイマーをデクリメント
	shotTimer--;
	//指定時間に達した
	if (shotTimer <= 0) {
		//弾を発射
		if (!player_->IsDead()) {
			Fire();
		}
		//発射タイマーを初期化
		shotTimer = kFireInterval;
	}
}

void Enemy::Fire() {
	assert(player_);
	assert(gameScene_);

	//弾の速度
	const float kBulletSpeed = 1.0f;

	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 worldPos = Subtract(playerPos, worldTransform_.translation_);
	Vector3 velocity = { Normalize(worldPos).x,Normalize(worldPos).y,Normalize(worldPos).z * kBulletSpeed };

	//速度ベクトルを自機の向きに合わせて回転させる
	//velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	//弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(modelBullet_, worldTransform_.translation_, velocity);
	//弾をゲームシーンに登録する
	gameScene_->AddEnemyBullet(newBullet);
}

void Enemy::OnCollision() {
	//死ぬ
	if (hp_ <= 0) {
		isDead_ = true;
	}
	hp_ -= 20;
}

Enemy::Enemy() {

}
Enemy::~Enemy() {

}

void Enemy::Initialize(Model* model, Model* modelBullet, uint32_t textureHandle, Vector3 pos) {
	assert(model);

	model_ = model;
	modelBullet_ = modelBullet;
	textureHandle_ = textureHandle;

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.translation_.y = 2.0f;
	//worldTransform_.rotation_.z = 90.0f;

	hp_ = 40; 
	deathTimer = 900;

	//Fire();

	//move = -0.3f;

	InitializeMoveGimmick();

	//接近フェーズ初期化
	ApproachInit();
	//メンバ関数ポインタに関数のアドレスを代入する
	pFunc = &Enemy::ApproachUpdate;
}

void Enemy::Update() {
	if (--deathTimer <= 0) {
		isDead_ = true;
	}

	//メンバ関数ポインタに入っている関数を呼び出す
	(this->*spFuncTable[static_cast<size_t>(phase_)])();

	//UpdateMoveGimmick();

	//キャラクターの座標を画面表示する処理
#ifdef _DEBUG

	ImGui::Begin("Enemy");
	ImGui::DragFloat3("Enemy.translation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Enemy.rotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::End();
#endif // _DEBUG

}

void Enemy::Draw(ViewProjection viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
//移動初期化
void Enemy::InitializeMoveGimmick() {
	//中心座標
	center = { 0.0f,1.5f,12.0f };
	//角度
	angle = { 0.5f,0.5f,0.5f };
	//半径の長さ
	length = 10.0f;
}

//移動更新
//void Enemy::UpdateMoveGimmick() {
//	// 中心座標に角度と長さを使用した円の位置を加算する
//	// 度数法の角度を弧度法に変換
//	float radius = angle.x * 3.14f / 180.0f;
//	// 三角関数を使用し、円の位置を割り出す。
//	float add_x = cos(radius) * length;
//	float add_z = sin(radius) * length;
//
//	//速さ
//	const float speed = 1.0f;
//	//移動量
//	Vector3 move = {
//		add_x,
//		0,
//		add_z
//	};
//	//移動量に速さを反映
//	move = Multiply(speed, Normalize(move));
//
//	//移動ベクトルをカメラの角度だけ回転する
//	Matrix4x4 rotate = MakeRotateMatrix(Multiply(1.0f, viewProjection_->rotation_));
//	move = TransformNormal(move, rotate);
//
//	//移動方向に向きを合わせる
//	//Y軸周り角度(θy)
//	worldTransform_.rotation_.y = std::atan2(move.x, move.z);
//
//	//結果ででた位置を中心位置に加算し、それを描画位置とする
//	worldTransform_.translation_.x = center.x + add_x;
//	worldTransform_.translation_.z = center.z + add_z;
//
//	// 角度更新
//	angle.x += 1.0f;
//
//	//発射タイマーをデクリメント
//	shotTimer--;
//	//指定時間に達した
//	if (shotTimer <= 0) {
//		//弾を発射
//		if (!player_->IsDead()) {
//			Fire();
//		}
//		//発射タイマーを初期化
//		shotTimer = kFireInterval;
//	}
//
//	ImGui::Begin("Enemy");
//	ImGui::SliderFloat3("body.translation", &worldTransform_.translation_.x, -10, 10);
//	/*ImGui::SliderFloat3("L_arm.translation", &worldTransformL_arm_.translation_.x, -10, 10);
//	ImGui::SliderFloat3("R_arm.translation", &worldTransformR_arm_.translation_.x, -10, 10);*/
//	ImGui::End();
//}
