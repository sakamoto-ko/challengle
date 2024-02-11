#pragma once
#include <functional>

class TimedCall {

public:
	TimedCall(std::function<void()> callBack, uint32_t time);

	void Update();

	bool IsFinished() { return isFinish; };

private:
	std::function<void()> callBack_;

	// 残りの時間
	uint32_t time_;

	// 完了のフラグ
	bool isFinish = false;
};