#pragma once

/// <summary>
/// Upgradeの種類
/// 1・新しい武器を装備
/// 2・既存の武器を強化
/// </summary>
enum class UpgradeType {
	NewWeapon,
	UpgradeWeapon,
};

/// <summary>
/// Upgradeの際に三択から選ぶときの選択肢
/// </summary>
enum class UpgradeSelect {
	Select1,
	Select2,
	Select3,
};