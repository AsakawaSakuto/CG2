#pragma once

/// <summary>
/// Upgradeの種類
/// 1・新しい武器を装備
/// 2・既存の武器のダメージを強化
/// 3・既存の武器の発射数を強化
/// 4・既存の武器のサイズを強化（AreaとToxic専用）
/// 5・既存の武器の持続時間を強化（AxeとToxic専用）
/// 6・既存の武器の貫通回数を強化（Laser専用）
/// 7・既存の武器の反射回数を強化（Runa専用）
/// </summary>
enum class UpgradeType {
	NewWeapon,
	UpgradeDamage,
	UpgradeShotMaxCount,
	UpgradeSize,             // AreaとToxic専用
	UpgradeLifeTime,         // AxeとToxic専用
	UpgradePenetrationCount, // Laser専用
	UpgradeBounceCount,      // Runa専用
};

/// <summary>
/// Upgradeの際に三択から選ぶときの選択肢
/// </summary>
enum class UpgradeSelect {
	Select1,
	Select2,
	Select3,
};