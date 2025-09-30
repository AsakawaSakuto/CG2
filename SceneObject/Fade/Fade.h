#pragma once
#include"Sprite.h"

using std::unique_ptr;
using std::make_unique;

class Fade
{
public:
	
	void Initialize(DirectXCommon* dxCommon);

	void Update();

	void Draw();

	void SetIsFade(bool isFade) { isFade_ = isFade; }

	void SetFadeInSpeed(float InSpeed) { fadeInSpeed_ = InSpeed; }

	void SetFadeOutSpeed(float OutSpeed) { fadeOutSpeed_ = OutSpeed; }

	bool GetIsFade() { return isFade_; }

	float GetFadeAlpha() { return fadeAlpha_; }
private:
	DirectXCommon* dxCommon_ = nullptr;

	unique_ptr<Sprite> loadingUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> fade_ = make_unique<Sprite>();

	float deltaTime_ = 1.0f / 60.0f;
	float fadeAlpha_ = 1.0f;
	bool isFade_ = false;

	float fadeInSpeed_ = 0.5f;
	float fadeOutSpeed_ = 0.5f;
};