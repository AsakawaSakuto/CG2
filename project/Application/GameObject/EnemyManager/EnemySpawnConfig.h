#pragma once
#include <algorithm>

/// <summary>
/// プレイヤーレベルに応じた敵のスポーン設定
/// </summary>
struct EnemySpawnConfig {
	// 基本ステータスの倍率
	float hpMultiplier = 1.0f;
	float powerMultiplier = 1.0f;
	float moveSpeedMultiplier = 1.0f;

	// スポーン関連
	float spawnInterval = 1.0f;      // 生成間隔（秒）
	int maxEnemyCount = 300;         // 最大敵数
	int enemiesPerSpawn = 1;         // 1回の生成で出現する敵の数

	// 経験値・お金のドロップ倍率
	float expMultiplier = 1.0f;
	float moneyMultiplier = 1.0f;

	/// <summary>
	/// プレイヤーレベルに基づいて設定を計算（緩やかなスケーリング）
	/// </summary>
	/// <param name="playerLevel">現在のプレイヤーレベル</param>
	/// <returns>計算されたスポーン設定</returns>
	static EnemySpawnConfig CalculateFromPlayerLevel(int playerLevel) {
		EnemySpawnConfig config;

		// レベル10ごとにフェーズが進む（より緩やかに）
		int phase = (playerLevel - 1) / 10;
		float levelProgress = static_cast<float>(playerLevel);

		// HP倍率: レベル1で1.0倍、レベル10で約1.45倍、レベル20で約1.95倍（緩やか）
		// 旧: 0.15倍/レベル → 新: 0.05倍/レベル
		config.hpMultiplier = 1.0f + (levelProgress - 1.0f) * 0.05f;

		// 攻撃力倍率: さらに緩やかに上昇
		// 旧: 0.08倍/レベル → 新: 0.03倍/レベル
		config.powerMultiplier = 1.0f + (levelProgress - 1.0f) * 0.03f;

		// 移動速度倍率: 上限1.3倍、非常に緩やかに
		// 旧: 0.05倍/レベル、上限1.5倍 → 新: 0.015倍/レベル、上限1.3倍
		config.moveSpeedMultiplier = (std::min)(1.0f + (levelProgress - 1.0f) * 0.015f, 1.3f);

		// スポーン間隔: レベルが上がるほど短く（最小0.5秒、より緩やかに）
		// 旧: 0.03秒/レベル減少、最小0.3秒 → 新: 0.015秒/レベル減少、最小0.5秒
		config.spawnInterval = (std::max)(1.0f - levelProgress * 0.015f, 0.5f);

		// 1回の生成で出現する敵の数: 10レベルごとに1体増加（最大3体）
		// 旧: 5レベルごと、最大5体 → 新: 10レベルごと、最大3体
		config.enemiesPerSpawn = (std::min)(1 + phase, 3);

		// 報酬も少し増やす（モチベーション維持）
		// 旧: 0.05倍/レベル → 新: 0.03倍/レベル
		config.expMultiplier = 1.0f + levelProgress * 0.03f;
		config.moneyMultiplier = 1.0f + levelProgress * 0.03f;

		return config;
	}
};
