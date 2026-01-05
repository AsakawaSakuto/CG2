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
	void SetChestCost(int cost) { chestCostFont_->SetNumber(cost); }

	/// <summary>
	/// 武器アイコンを更新する
	/// </summary>
	/// <param name="slotIndex">武器スロット番号（0-3）</param>
	/// <param name="weaponName">武器の種類</param>
	void UpdateWeaponIcon(int slotIndex, WeaponName weaponName);

	void SetPlayTime(float time) { playTime_ = time; }

	void SetWeaponLv(int slotIndex, int lv) {
		if (slotIndex == 0) {
			weaponLvFont1_->SetNumber(lv);
		}
		if (slotIndex == 1) {
			weaponLvFont2_->SetNumber(lv);
		}
		if (slotIndex == 2) {
			weaponLvFont3_->SetNumber(lv);
		}
		if (slotIndex == 3) {
			weaponLvFont4_->SetNumber(lv);
		}
	}
	
	void SetWeaponEquipped(int slotIndex, bool equipped) {
		if (slotIndex >= 0 && slotIndex < 4) {
			isWeaponEquipped_[slotIndex] = equipped;
		}
	}
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

	std::unique_ptr<BitmapFont> playTimeFont_;
	float playTime_ = 0.0f;

	std::unique_ptr<Sprite> chestIcon_;
	unique_ptr<BitmapFont> chestCostFont_;

	std::unique_ptr<Sprite> weaponLv1_;
	std::unique_ptr<Sprite> weaponLv2_;
	std::unique_ptr<Sprite> weaponLv3_;
	std::unique_ptr<Sprite> weaponLv4_;
	unique_ptr<BitmapFont> weaponLvFont1_;
	unique_ptr<BitmapFont> weaponLvFont2_;
	unique_ptr<BitmapFont> weaponLvFont3_;
	unique_ptr<BitmapFont> weaponLvFont4_;

	// 各スロットに武器が装備されているかどうか
	bool isWeaponEquipped_[4] = { false, false, false, false };

	/// <summary>
	/// 武器名からアイコンのテクスチャパスを取得
	/// </summary>
	std::string GetWeaponIconPath(WeaponName weaponName) const;
};