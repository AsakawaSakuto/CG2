#pragma once
#include "GameUI/BaseUI.h"
#include "EngineSystem.h"

class GameSceneUI : public BaseUI {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;

	void SetNowMoney(int money) { moneyValue_ = money; }
	void SetExpGauge(float currentExp, float maxExp) { currentExpValue_ = currentExp; maxExpValue_ = maxExp; }
	void SetHpGauge(float currentHp, float maxHp) { currentHpValue_ = currentHp; maxHpValue_ = maxHp; }
	void SetNowLv(int lv) { nowLv_ = lv; }
	void SetKillEnemyCount(int count) { killEnemyValue_ = count; }
private:
	std::unique_ptr<BitmapFont> enemyFont_;
	std::unique_ptr<Sprite> enemy_;
	int killEnemyValue_ = 0;


	std::unique_ptr<BitmapFont> moneyFont_;
	std::unique_ptr<Sprite> money_;
	int moneyValue_ = 0;

	std::unique_ptr<BitmapFont> lvFont_;
	std::unique_ptr<Sprite> lv_;
	int nowLv_ = 1;

	std::unique_ptr<Gauge> expGauge_;
	float currentExpValue_ = 0.0f;
	float maxExpValue_ = 100.0f;

	std::unique_ptr<Gauge> hpGauge_;
	std::unique_ptr<BitmapFont> currentHpFont_;
	std::unique_ptr<BitmapFont> maxHpFont_;
	std::unique_ptr<Sprite> hpSrash_;
	float currentHpValue_ = 0.0f;
	float maxHpValue_ = 100.0f;
};