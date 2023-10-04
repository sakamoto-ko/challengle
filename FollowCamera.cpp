#include "FollowCamera.h"
#include "MyMath.h"
#include <cassert>
#include "ImGuiManager.h"

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
	//ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float rotate = 0.03f; 
		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * rotate;
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

	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &viewProjection_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.01f);
	ImGui::End();
#endif // _DEBUG

}