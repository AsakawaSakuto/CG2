#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct BulletState {
	float maxSpeed = 12.0f; // プレイヤーの最高速度
	int bulletGaugeMax = 5; // 弾の最大数

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(BulletState, maxSpeed, bulletGaugeMax);
};