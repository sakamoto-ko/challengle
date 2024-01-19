#include "FollowCamera.h"
#include "MyMath.h"
#include <cassert>
#include "ImGuiManager.h"

#include "LockOn.h"

FollowCamera::FollowCamera() {}
FollowCamera::~FollowCamera() {}
//初期化
void FollowCamera::Initialize() {
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}
//更新
void FollowCamera::Update() {
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	//ロックオン中であれば	
	if (lockOn_ && lockOn_->ExistTarget()) {
		//カメラをロックオン対象に向ける
		//ロックオン座標取得
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		//追従対象からロックオン対象へのベクトル	
		Vector3 sub = Subtract(lockOnPosition, target_->translation_);

		//Y軸周り角度
		viewProjection_.rotation_.y = std::atan2(sub.x, sub.z);
	}
	else {
		//スティックで角度を取得
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			const float rotate = 0.01f;
			viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * rotate;
		}
	}

	//追従対象がいれば
	if (target_) {
		//追従対象からカメラまでのオフセット
		Vector3 offset = { 0.0f,2.0f,-10.0f };

		Matrix4x4 rotate = MakeRotateMatrix(viewProjection_.rotation_);

		//オフセットをカメラの回転に合わせて回転させる
		offset = TransformNormal(offset, rotate);

		//座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = Add(target_->translation_, offset);

		viewProjection_.UpdateMatrix();
		viewProjection_.TransferMatrix();
	}

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("Camera")) {
		ImGui::DragFloat3("translation", &viewProjection_.translation_.x, 0.01f);
		ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}
