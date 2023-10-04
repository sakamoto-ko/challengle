#pragma once
#include "Sprite.h"
#include "TextureManager.h"

class TransitionEffect
{
private:
	//テクスチャハンドル
	uint32_t textureHandle_[2] = { 0u,0u };

	// スプライト
	std::unique_ptr<Sprite> transition_[2];

	Vector2 leftMove = {};
	Vector2 rightMove = {};

	Vector2 velocity = {};
	Vector2 accelelate = {};

	int32_t boundCount_ = 0;

	float addAccelelate = 0.03f;

	int count_ = 0;

	bool isFadeIn_ = false;
	bool isFadeOut_ = false;

	bool isSceneChange = false;

public:
	static TransitionEffect* GetInstance();

	void Init();
	void Update();
	void Draw();

	bool GetIsChangeScene() { return isSceneChange; }

	void SetIsChangeScene(bool flag) { isSceneChange = flag; }

	void Reset();

	//フェードインアウトゲッター関数
	bool GetFadeIn() { return isFadeIn_; }
	bool GetFadeOut() { return isFadeOut_; }

};

