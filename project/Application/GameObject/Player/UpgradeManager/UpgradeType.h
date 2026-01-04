#pragma once

/// <summary>
/// Upgradeの種類
/// 1・新しい武器を装備
/// 2・既存の武器のダメージを強化
/// 3・既存の武器の発射数を強化
/// </summary>
enum class UpgradeType {
	NewWeapon,
	UpgradeDamage,
	UpgradeShotMaxCount,
};

/// <summary>
/// Upgradeの際に三択から選ぶときの選択肢
/// </summary>
enum class UpgradeSelect {
	Select1,
	Select2,
	Select3,
};