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

bool Button::isTriggerSpace() {
	if (Gamepad::Pushed(Gamepad::Button::A)) {
		return true;
	}
	else if (input_->TriggerKey(DIK_SPACE)) {
		return true;
	}

	return false;
}

bool Button::isPressSpace() {
	if (Gamepad::LongPush(Gamepad::Button::A)) {
		return true;
	}
	else if (input_->PushKey(DIK_SPACE)) {
		return true;
	}

	return false;
}
