#pragma once
#include <nlohmann/json.hpp>

struct ScoreList {
	float shotHitAmount;     // 敵に弾を当てた時の加算スコア
	float wingHitNearAmount; // 羽に当てた時の加算スコア (近)
	float wingHitFarAmount;  // 羽に当てた時の加算スコア (遠)
	float enemyHitAmount;    // 下降時に敵に当たった時の加算スコア
	float stunAmount;        // 上昇時に敵に当たった時の減算スコア

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScoreList, shotHitAmount, wingHitNearAmount, wingHitFarAmount, enemyHitAmount, stunAmount);
};