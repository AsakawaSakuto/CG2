#pragma once
#include "EngineSystem.h"
#include "UpgradeType.h"
#include "GameObject/Rarity.h"
#include "GameObject/Player/WeaponManager/WeaponStatus.h"
#include <vector>
#include <array>

// 前方宣言
class WeaponManager;

/// <summary>
/// アップグレードの選択肢情報
/// </summary>
struct UpgradeOption {
	UpgradeType type = UpgradeType::NewWeapon;  // 新規装備 or 強化
	WeaponName weaponName = WeaponName::None;   // 対象武器
	Rarity rarity = Rarity::UnCommon;           // レアリティ
};

class UpgradeManager {
public:

	void Initialize();

	void Update();

	void Draw();

	void DrawImGui();

	/// <summary>
	/// Playerがレベルアップしたときに呼び出す
	/// 強化の抽選を開始する
	/// まだ装備していない武器と、装備してる武器の強化を選択肢に含める
	/// 選択肢に出てくる武器はランダムに決定するが3つの選択肢内で被ることはない
	/// </summary>
	void Upgrade();

	bool IsUpgradeSelect() const { return isUpgrade_; }

	/// <summary>
	/// WeaponManagerを設定
	/// </summary>
	void SetWeaponManager(WeaponManager* weaponManager) { weaponManager_ = weaponManager; }

	/// <summary>
	/// 選択されたアップグレードオプションを取得
	/// </summary>
	const UpgradeOption& GetSelectedOption() const;

private:
	/// <summary>
	/// レアリティをランダムに決定
	/// UnCommon 50%, Rare 37%, Epic 10%, Legendary 3%
	/// </summary>
	Rarity RandomRarity();

	/// <summary>
	/// 3つのアップグレード選択肢を生成
	/// </summary>
	void GenerateUpgradeOptions();

	/// <summary>
	/// レアリティに応じた背景色を取得
	/// </summary>
	Vector4 GetRarityColor(Rarity rarity);

	/// <summary>
	/// 武器名からアイコンのテクスチャパスを取得
	/// </summary>
	std::string GetWeaponIconPath(WeaponName weaponName) const;

	/// <summary>
	/// 武器名から名前テクスチャのパスを取得
	/// </summary>
	std::string GetWeaponNamePath(WeaponName weaponName) const;

	/// <summary>
	/// UIを選択肢に応じて更新
	/// </summary>
	void UpdateUpgradeUI();

	/// <summary>
	/// 選択を確定して強化を適用
	/// </summary>
	void ApplySelectedUpgrade();

private:
	// アップグレード選択中かどうか
	bool isUpgrade_ = false;

	UpgradeSelect upgradeSelect_ = UpgradeSelect::Select1;

	// 3つのアップグレード選択肢
	std::array<UpgradeOption, 3> upgradeOptions_;

	// WeaponManagerへの参照
	WeaponManager* weaponManager_ = nullptr;

	std::unique_ptr<Sprite> upgradeSelectText_;
	std::unique_ptr<Sprite> upgradeBG1_;
	std::unique_ptr<Sprite> upgradeBG2_;
	std::unique_ptr<Sprite> upgradeBG3_;
	std::unique_ptr<Sprite> upgradeIcon1_;
	std::unique_ptr<Sprite> upgradeIcon2_;
	std::unique_ptr<Sprite> upgradeIcon3_;
	std::unique_ptr<Sprite> upgradeName1_;
	std::unique_ptr<Sprite> upgradeName2_;
	std::unique_ptr<Sprite> upgradeName3_;
	std::unique_ptr<Sprite> newText1_;
	std::unique_ptr<Sprite> newText2_;
	std::unique_ptr<Sprite> newText3_;
	std::unique_ptr<Sprite> bg_;
};