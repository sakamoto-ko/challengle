#include "TransitionEffect.h"
#include "MyMath.h"

#include <cassert>
#include <list>

TransitionEffect* TransitionEffect::GetInstance() {

	static TransitionEffect instance;
	return &instance;

}

void TransitionEffect::Init() {
	textureHandle_[0] = TextureManager::Load("hud/leftDoor.png");
	textureHandle_[1] = TextureManager::Load("hud/rightDoor.png");

	//スプライト生成
	for (int i = 0; i < 2; i++) {
		transition_[i].reset(Sprite::Create(textureHandle_[i], { 0.0f,0.0f }));
		transition_[i]->SetSize({ 640.0f, 720.0f });
		transition_[i]->SetTextureRect({ 0.0f, 0.0f }, { 131.0f, 251.0f });
	}

	leftMove = { -640.0f,0.0f };
	rightMove = { 1280.0f,0.0f };

	transition_[0]->SetPosition({ -640.0f,0.0f });
	transition_[1]->SetPosition({ 1280.0f,0.0f });

	velocity = { 1.0f,0.0f };
	accelelate = { 0.0f,0.0f };
	boundCount_ = 0;
	addAccelelate = 0.03f;
	isFadeIn_ = true;
	isFadeOut_ = false;
	count_ = 0;
}

void TransitionEffect::Update() {

	if (isSceneChange) {
		if (isFadeIn_) {
			accelelate.x += addAccelelate;
			velocity.x += accelelate.x;

			if (transition_[0]->GetPosition().x + 640.0f >= transition_[1]->GetPosition().x) {
				boundCount_++;
				if (boundCount_ <= 2) {
					velocity.x *= -1.0f;
				}
				else {
					velocity.x *= 0.0f;
					addAccelelate = 0.0f;
					isFadeIn_ = false;
					isFadeOut_ = true;
				}
			}
			if (boundCount_ <= 2) {
				addAccelelate = 0.02f;
			}
			else {
				addAccelelate -= 0.01f;
			}

			leftMove.x += velocity.x;
			rightMove.x -= velocity.x;
		}
		else if (isFadeOut_) {
			count_++;
			if (count_ >= 30) {
				addAccelelate = 0.03f;

				accelelate.x += addAccelelate;
				velocity.x += accelelate.x;

				leftMove.x -= velocity.x;
				rightMove.x += velocity.x;

				if (transition_[0]->GetPosition().x <= 0.0f &&
					transition_[1]->GetPosition().x >= 1280.0f) {
					isFadeOut_ = false;
					isSceneChange = false;
					count_ = 0;
				}
			}
		}
		leftMove.x = Clamp(leftMove.x, -640.0f, 0.0f);
		rightMove.x = Clamp(rightMove.x, 640.0f, 1280.0f);

		transition_[0]->SetPosition({ leftMove.x,leftMove.y });
		transition_[1]->SetPosition({ rightMove.x,rightMove.y });
	}
}

void TransitionEffect::Draw() {
	if (isSceneChange) {
		transition_[0]->Draw();
		transition_[1]->Draw();
	}
}

void TransitionEffect::Reset() {

	leftMove = { -640.0f,0.0f };
	rightMove = { 1280.0f,0.0f };

	transition_[0]->SetPosition({ -640.0f,0.0f });
	transition_[1]->SetPosition({ 1280.0f,0.0f });

	velocity = { 1.0f,0.0f };
	accelelate = { 0.0f,0.0f };
	boundCount_ = 0;
	addAccelelate = 0.03f;
	isFadeIn_ = true;
	isFadeOut_ = false;
	count_ = 0;
}