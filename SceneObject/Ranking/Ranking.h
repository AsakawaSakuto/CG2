#pragma once
#include"Sprite.h"

using std::unique_ptr;
using std::make_unique;

class Ranking
{
public:

	void Initialize(DirectXCommon* dxCommon);

	void Update();

	void Draw();

	void UpdateRanking();

	void DrawRanking();

	void DrawImGui();

	void IsEndGame();
private:
	DirectXCommon* dxCommon_ = nullptr;

	unique_ptr<Sprite> timerNum1_ = make_unique<Sprite>();
	unique_ptr<Sprite> timerNum2_ = make_unique<Sprite>();
	unique_ptr<Sprite> timerNum3_ = make_unique<Sprite>();
	unique_ptr<Sprite> timerColon_ = make_unique<Sprite>();

	unique_ptr<Sprite> top1Num1_ = make_unique<Sprite>();
	unique_ptr<Sprite> top1Num2_ = make_unique<Sprite>();
	unique_ptr<Sprite> top1Num3_ = make_unique<Sprite>();
	unique_ptr<Sprite> top1Colon_ = make_unique<Sprite>();

	unique_ptr<Sprite> top2Num1_ = make_unique<Sprite>();
	unique_ptr<Sprite> top2Num2_ = make_unique<Sprite>();
	unique_ptr<Sprite> top2Num3_ = make_unique<Sprite>();
	unique_ptr<Sprite> top2Colon_ = make_unique<Sprite>();

	unique_ptr<Sprite> top3Num1_ = make_unique<Sprite>();
	unique_ptr<Sprite> top3Num2_ = make_unique<Sprite>();
	unique_ptr<Sprite> top3Num3_ = make_unique<Sprite>();
	unique_ptr<Sprite> top3Colon_ = make_unique<Sprite>();

	const float deltaTime_ = 1.0f / 60.0f;
	float timer_ = 0.0f;
	float seconds_ = 0.0f;
	int minutes_ = 0;
	int totalSeconds_ = 0;

	const char* numberTextures[10] = {
	"resources/image/number/0.png",
	"resources/image/number/1.png",
	"resources/image/number/2.png",
	"resources/image/number/3.png",
	"resources/image/number/4.png",
	"resources/image/number/5.png",
	"resources/image/number/6.png",
	"resources/image/number/7.png",
	"resources/image/number/8.png",
	"resources/image/number/9.png"
	};
};