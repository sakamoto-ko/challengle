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

#ifdef _DEBUG

	ImGui::Begin("window");
	if (ImGui::TreeNode("button")) {
		ImGui::Text("Press %d", isPress());
		ImGui::Text("Trigger %d", isTrigger());
		ImGui::TreePop();
	}
	ImGui::End();

#endif // _DEBUG
}

bool Button::isTrigger() {
	if (Gamepad::Pushed(Gamepad::Button::A)) {
		return true;
	}
	else if (input_->TriggerKey(DIK_SPACE)) {
		return true;
	}

	return false;
}

bool Button::isPress() {
	if (Gamepad::LongPush(Gamepad::Button::A)) {
		return true;
	}
	else if (input_->PushKey(DIK_SPACE)) {
		return true;
	}

	return false;
}
