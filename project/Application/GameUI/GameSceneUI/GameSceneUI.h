#pragma once
#include "GameUI/BaseUI.h"
#include "EngineSystem.h"

class GameSceneUI : public BaseUI {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;

	void SetNowMoney(int money) { nowMoneyValue_ = money; }
	void SetExpGauge(float currentExp, float maxExp) { currentExpValue_ = currentExp; maxExpValue_ = maxExp; }

private:
	std::unique_ptr<BitmapFont> nowMoneyFont_;
	int nowMoneyValue_ = 0;

	std::unique_ptr<Gauge> expGauge_;
	float currentExpValue_ = 0.0f;
	float maxExpValue_ = 100.0f;
};