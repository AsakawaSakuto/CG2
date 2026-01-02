#pragma once
#include "GameUI/BaseUI.h"
#include "EngineSystem.h"
#include "GameObject/Player/WeaponManager/WeaponStatus.h"

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

	/// <summary>
	/// 武器アイコンを更新する
	/// </summary>
	/// <param name="slotIndex">武器スロット番号（0-3）</param>
	/// <param name="weaponName">武器の種類</param>
	void UpdateWeaponIcon(int slotIndex, WeaponName weaponName);

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

	std::unique_ptr<Sprite> weaponIcon1_;
	std::unique_ptr<Sprite> weaponIcon2_;
	std::unique_ptr<Sprite> weaponIcon3_;
	std::unique_ptr<Sprite> weaponIcon4_;

	/// <summary>
	/// 武器名からアイコンのテクスチャパスを取得
	/// </summary>
	std::string GetWeaponIconPath(WeaponName weaponName) const;
};