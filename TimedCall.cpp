#include "TimedCall.h"

TimedCall::TimedCall(std::function<void()> callBack, uint32_t time) {

	this->callBack_ = callBack;
	this->time_ = time;
}

void TimedCall::Update() {

	if (isFinish) {
		return;
	}

	time_--;

	if (time_ <= 0) {

		isFinish = true;

		// コールバックの呼び出し
		callBack_();
	}
}