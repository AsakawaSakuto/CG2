#pragma once
#include "Sprite.h"
#include "Application/EngineSystem.h"
#include "Application/GameUI/GameUI.h"
#include "Engine/System/Utility/GameTimer/GameTimer.h"

using std::unique_ptr;
using std::make_unique;

class GameTimer;

class SceneFade
{
public:
	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw();

	bool EndFadeIn() { return fadeInTimer_.IsFinished(); }
	bool EndFadeOut() { return fadeOutTimer_.IsFinished(); }

private:
	DirectXCommon* dxCommon_ = nullptr;

	float fadeAlpha_ = 0.0f;

	GameTimer fadeInTimer_;
	GameTimer fadeOutTimer_;

	unique_ptr<Sprite> backBlack_ = make_unique<Sprite>();
	unique_ptr<Sprite> loadingText_ = make_unique<Sprite>();
};