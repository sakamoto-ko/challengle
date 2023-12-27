#include "LockOn.h"
#include "Enemy.h"
#include <cassert>

LockOn::LockOn() {}

LockOn::~LockOn() {}

void LockOn::Initialize() {
	//テクスチャ
	lockOnMarkTex_ = TextureManager::Load("lockOn.png");
	//スプライト
	sprite2DReticle_.reset(Sprite::Create(lockOnMarkTex_, { 0.0f, 0.0f }));
	sprite2DReticle_->SetSize({ 96.0f, 96.0f });
	sprite2DReticle_->SetTextureRect({ 0.0f,0.0f, }, { 1500.0f, 1500.0f });
	sprite2DReticle_->SetPosition({ 640.0f, 360.0f });
}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {
	//ロックオン中なら
	if (target_) {
		//Lトリガーを押していたら
		if (TriggerL()) {
			//ロックオン解除
			target_ = nullptr;
		}
		//範囲外設定
		else if (OutOfRange(viewProjection)) {
			target_ = nullptr;
		}
	}
	else {
		//Lトリガーを押していたら
		if (TriggerL()) {
			//ロックオン対象を検索
			LockOnTargetSerch(enemies, viewProjection);
		}
	}

	//ロックオン継続
	if (target_) {
		/*ロックオンマークの座標計算*/
		//敵のロックオン座標取得
		Vector3 positionWorld = GetLockOnPosition();
		//ワールド座標からスクリーン座標に変換
		Vector3 positionScreen = ConvertScreen(positionWorld, viewProjection);
		//Vector2に格納
		Vector2 positionScreenV2(positionScreen.x, positionScreen.y);
		//スプライトの座標を設定
		sprite2DReticle_->SetPosition(positionScreenV2);
	}
}

void LockOn::Draw() {
	//ロックオンしていれば
	if (target_) {
		sprite2DReticle_->Draw();
	}
}

void LockOn::LockOnTargetSerch(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {
	/*ロックオン対象の絞り込み*/
	//目標
	std::list<std::pair<float, Enemy*>> targets;

	//全ての敵に対して順にロックオン判定
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		//敵のロックオン座標取得
		Vector3 positionWorld = enemy->GetCenterPosition();

		//ワールド→ビュー座標変換
		Vector3 positionView = ConvertView(positionWorld, viewProjection);

		//条件チェック
		if (CheckAllCondition(positionView)) {
			targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
		}

		/*距離でソートしてロックオン対象を決定*/
		//ロックオン対象をリセット
		target_ = nullptr;

		if (!targets.empty()) {
			//距離で昇順にソート
			targets.sort([](auto pair1, auto& pair2) { return pair1.first < pair2.first; });
			//ソートの結果一番近い敵をロックオン対象とする
			target_ = targets.front().second;
		}
	}
}

bool LockOn::OutOfRange(const ViewProjection& viewProjection) {
	//敵のロックオン座標取得
	Vector3 positionWorld = GetLockOnPosition();
	//ワールド→ビュー座標変換
	Vector3 positionView = ConvertView(positionWorld, viewProjection);

	//条件チェック
	if (CheckAllCondition(positionView)) {
		//範囲外ではない
		return false;
	}
	//範囲外である
	return true;
}

bool LockOn::CheckDistanceCondition(Vector3 positionView)
{
	if (minDistance_ <= positionView.z &&
		positionView.z <= maxDistance_) {
		return true;
	}
	return false;
}

bool LockOn::CheckAngleCondition(Vector3 positionView)
{
	//カメラ前方との角度を計算
	float arcTangent = std::atan2(
		std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y),
		positionView.z
	);

	if (std::sqrt(arcTangent * arcTangent) <= angleRenge_) {
		return true;
	}
	return false;
}

bool LockOn::CheckAllCondition(Vector3 positionView)
{
	if (CheckDistanceCondition(positionView)) {
		if (CheckAngleCondition(positionView)) {
			return true;
		}
	}
	return false;
}

Vector3 LockOn::GetLockOnPosition()
{
	//敵のロックオン座標取得
	Vector3 positionWorld = target_->GetCenterPosition();

	return positionWorld;
}

Vector3 LockOn::ConvertScreen(Vector3 pos, const ViewProjection& viewProjection)
{
	//ワールド→スクリーン変換
	Vector3 positionScreen = ConvertScreenPosition(viewProjection, pos);

	return positionScreen;
}

Vector3 LockOn::ConvertView(Vector3 pos, const ViewProjection& viewProjection) {
	//ワールド→ビュー座標変換
	Vector3 positionView = TransformNormal(pos, viewProjection.matView);

	return positionView;
}

bool LockOn::TriggerL() {
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	//ゲームパッドに接続されている
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//Lトリガーを押していたら
		//前フレームの状態がfalse
		if (Input::GetInstance()->GetJoystickStatePrevious((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER), joyState)) {
			//現在フレームの状態がtrue
			if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == 256) {
				return true;
			}
		}
	}

	return false;
}
