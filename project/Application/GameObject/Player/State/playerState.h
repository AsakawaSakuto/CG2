#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct PlayerState {
	float maxSpeed = 6.0f;                // プレイヤーの最高速度
	float cameraOffset = 4.0f;            // カメラのオフセット
	float stunDuration = 1.0f;            // スタンする時間(秒)
	float speedDownStrengthThorn = 2.0f;  // スタン時の減速の強さトゲとの衝突時
	float speedDownStrengthBullet = 1.0f; // スタン時の減速の強さ弾を撃った時
	float shakeStrength = 0.5f;           // スタン時の画面揺れの強さ
	int maxCoolDownWing = 60;             // プレイヤー被弾時のクールダウン

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerState, maxSpeed, cameraOffset, stunDuration, speedDownStrengthThorn, speedDownStrengthBullet, shakeStrength);
};
