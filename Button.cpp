#include "Button.h"
#include "Gamepad.h"
#include <assert.h>

Button::Button() {
	this->Initialize();
}

Button::~Button() {}

void Button::Initialize() {
	input_ = Input::GetInstance();
}

void Button::Update() {

	// ゲームパッドのインスタンスを取得
	Gamepad::Input();
}

bool Button::isTriggerJump() {
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			return true;
		}
	}
	if (input_->TriggerKey(DIK_SPACE)) {
		return true;
	}

	return false;
}
bool Button::isTriggerAttack() {
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	//前フレームの状態がfalse
	if (Input::GetInstance()->GetJoystickStatePrevious((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER), joyState)) {
		//現在フレームの状態がtrue
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) >= 128) {
			return true;
		}
	}
	if (input_->TriggerKey(DIK_RETURN)) {
		return true;
	}

	return false;
}

bool Button::isTriggerRight(){
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//前フレームの状態がfalse
		if (Input::GetInstance()->GetJoystickStatePrevious(joyState.Gamepad.sThumbLX / SHRT_MAX, joyState)) {
			//現在フレームの状態がtrue
			if (joyState.Gamepad.sThumbLX / SHRT_MAX >= 0.5) {
				return true;
			}
		}
	}
	if (input_->TriggerKey(DIK_RIGHT) ||
		input_->TriggerKey(DIK_D)) {
		return true;
	}

	return false;
}

bool Button::isTriggerLeft() {
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//前フレームの状態がfalse
		if (Input::GetInstance()->GetJoystickStatePrevious(joyState.Gamepad.sThumbLX / SHRT_MAX, joyState)) {
			//現在フレームの状態がtrue
			if (joyState.Gamepad.sThumbLX / SHRT_MAX <= -0.5) {
				return true;
			}
		}
	}
	if (input_->TriggerKey(DIK_LEFT) ||
		input_->TriggerKey(DIK_A)) {
		return true;
	}

	return false;
}

bool Button::isTriggerUp() {
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//前フレームの状態がfalse
		if (Input::GetInstance()->GetJoystickStatePrevious(joyState.Gamepad.sThumbLY / SHRT_MAX, joyState)) {
			//現在フレームの状態がtrue
			if (joyState.Gamepad.sThumbLY / SHRT_MAX >= 0.5) {
				return true;
			}
		}
	}
	if (input_->TriggerKey(DIK_UP) ||
		input_->TriggerKey(DIK_W)) {
		return true;
	}

	return false;
}

bool Button::isTriggerDown() {
	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;
	
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//前フレームの状態がfalse
		if (Input::GetInstance()->GetJoystickStatePrevious(joyState.Gamepad.sThumbLY / SHRT_MAX, joyState)) {
			//現在フレームの状態がtrue
			if (joyState.Gamepad.sThumbLY / SHRT_MAX <= -0.5) {
				return true;
			}
		}
	}
	if (input_->TriggerKey(DIK_DOWN) ||
		input_->TriggerKey(DIK_S)) {
		return true;
	}

	return false;
}

//bool Button::isPressJump() {
//	if (Gamepad::LongPush(Gamepad::Button::A)) {
//		return true;
//	}
//	else if (input_->PushKey(DIK_SPACE)) {
//		return true;
//	}
//
//	return false;
//}
//
//bool Button::isPressAttack() {
//	if (Gamepad::LongPush(Gamepad::Button::RIGHT_SHOULDER)) {
//		return true;
//	}
//	else if (input_->PushKey(DIK_RETURN)) {
//		return true;
//	}
//
//	return false;
//}
