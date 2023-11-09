#include "tempo.h"

void tempo::Initialize()
{
	Reset();
}

void tempo::Update()
{
	if (++time >= onTime) {
		tempo_ = true;
		if (++time >= offTime) {
			tempo_ = false;
			time = 0;
		}
	}
#ifdef _DEBUG

	ImGui::Begin("window");
		if (ImGui::TreeNode("tempo")) {
			ImGui::Text("tempo %d", tempo_);
			ImGui::TreePop();
		}
	ImGui::End();

#endif // _DEBUG
}

void tempo::Reset()
{
	tempo_ = false;
	time = 0;
	onTime = 30;
	offTime = 60;
}
