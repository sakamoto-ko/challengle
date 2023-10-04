#include "RailCamera.h"

#include "MyMath.h"

#include <cassert>
#include <list>

RailCamera::RailCamera() {}
RailCamera::~RailCamera() {}

//初期化
void RailCamera::Initialize(WorldTransform worldTransform, Vector3 radian) {
	//引数でワールド座標を受け取ってワールドトランスフォームに設定
	worldTransform_ = worldTransform;
	//引数で回転角[ラジアン]を受け取ってワールドトランスフォームに設定
	worldTransform_.rotation_ = radian;
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	viewProjection_.farZ = 10000.0f;

	//シングルトンインスタンスを取得する 
	input_ = Input::GetInstance();
	kMoveSpeed.z = 0.0f;
	//kRotSpeed.y = (1.0f/360.0f);
}
//更新
void RailCamera::Update() {
	//XINPUT_STATE joyState;

	worldTransform_.translation_ = Add(worldTransform_.translation_, kMoveSpeed);
	
	worldTransform_.rotation_ = Add(kRotSpeed,worldTransform_.rotation_ );

	//ワールドトランスフォームのワールド座標再計算
	worldTransform_.UpdateMatrix();

	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	//カメラの座標を画面表示させる処理
#ifdef _DEBUG

	ImGui::Begin("Camera");
	ImGui::DragFloat3("Camera.translation", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Camera.rotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::End();
#endif // _DEBUG

}