#include "TransitionEffect.h"
#include "MyMath.h"

#include <cassert>
#include <list>

TransitionEffect* TransitionEffect::GetInstance() {

	static TransitionEffect instance;
	return &instance;

}

void TransitionEffect::Init() {
	textureHandle_[0] = TextureManager::Load("hud/downTriangle.png");
	textureHandle_[1] = TextureManager::Load("hud/upTriangle.png");

	//スプライト生成
	for (int i = 0; i < 2; i++) {
		transition_[i].reset(Sprite::Create(textureHandle_[i], { 0.0f,0.0f }));
		transition_[i]->SetSize({ 1280.0f, 720.0f });
		transition_[i]->SetTextureRect({ 0.0f, 0.0f }, { 1280.0f, 720.0f });
		transition_[i]->SetPosition({ 0.0f,-720.0f });
	}

	downMove = { 0.0f,-720.0f };
	upMove = { 0.0f,-640.0f };

	velocity = { 0.0f,8.0f };
	accelelate = { 0.0f,0.0f };
	addAccelelate = 0.1f;
	isFadeIn_ = true;
	isFadeOut_ = false;
	isNext = false;
	count_ = 0;
}

void TransitionEffect::Update() {

	if (isSceneChange) {
		if (isFadeIn_) {
			if (!isNext) {
				if (velocity.y <= 0.0f) {
					if (++count_ >= 20) {
						velocity = { 0.0f,10.0f };
						upMove = { 0.0f,-600.0f };
						isNext = true;
						count_ = 0;
					}
				}
				else {
					velocity.y -= addAccelelate;

					downMove.y += velocity.y;
				}
			}
			else {
				downMove.y += velocity.y;
				upMove.y += velocity.y;

				if (transition_[0]->GetPosition().y + 720.0f >= 720.0f) {
					isFadeIn_ = false;
					isFadeOut_ = true;
					isNext = false;
					addAccelelate = 0.08f;
				}
			}
		}
		else if (isFadeOut_) {

			if (!isNext) {
				if (velocity.y <= 0.0f) {
					if (++count_ >= 20) {
						velocity = { 0.0f,10.0f };
						isNext = true;
						count_ = 0;
					}
				}
				else {
					velocity.y -= addAccelelate;
					downMove.y += velocity.y;
					upMove.y += velocity.y;
				}
			}
			else {
				downMove.y += velocity.y;
				upMove.y += velocity.y;

				if (transition_[1]->GetPosition().y >= 720.0f) {
					isFadeOut_ = false;
					isSceneChange = false;
					isNext = false;
				}
			}
		}
	}

	//upMove.y = Clamp(upMove.y, -1280.0f, 1280.0f);
	//downMove.y = Clamp(downMove.y, -1280.0f, 1280.0f);

	transition_[0]->SetPosition({ 0.0f,downMove.y });
	transition_[1]->SetPosition({ 0.0f,upMove.y });
}

void TransitionEffect::Draw() {
	if (isSceneChange) {
		transition_[0]->Draw();
		transition_[1]->Draw();
	}
}

void TransitionEffect::Reset() {

	transition_[0]->SetPosition({ 0.0f,-720.0f });
	transition_[1]->SetPosition({ 0.0f,-720.0f });

	downMove = { 0.0f,-720.0f };
	upMove = { 0.0f,-720.0f };

	velocity = { 0.0f,10.0f };
	accelelate = { 0.0f,0.0f };
	addAccelelate = 0.1f;
	isFadeIn_ = true;
	isFadeOut_ = false;
	isNext = false;
	count_ = 0;
}