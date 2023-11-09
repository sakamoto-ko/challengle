#pragma once

#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "SafeDelete.h"
#include <Windows.h>

typedef enum {
	A, // A

	END = 1,
}Keypad;

class Button final {
private:
	Button();

	// 入力
	Input* input_ = nullptr;

public:
	~Button();

	// コピーや代入演算子を無効化
	Button(const Button& button) = delete;
	Button& operator=(const Button& button) = delete;

	//シングルトンインスタンスを取得
	static Button* GetInstance() {
		static Button instance;
		return &instance;
	}

	void Initialize();
	void Update();

	//トリガーチェック
	bool isTriggerJump();
	bool isTriggerAttack();
	bool isTriggerRight();
	bool isTriggerLeft();

	//プレスチェック
	bool isPressJump();
	bool isPressAttack();
};