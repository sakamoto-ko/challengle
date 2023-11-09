#pragma once
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "SafeDelete.h"

class tempo
{
public:
	void Initialize();
	void Update();
	bool GetTempo() { return tempo_; }
	void Reset();
private:
	bool tempo_ = false;
	int time = 0;
	int onTime = 10;
	int offTime = 30;
};

