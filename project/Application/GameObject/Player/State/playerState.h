#pragma once
#include <string>

struct PlayerState {
	float maxSpeed = 6.0f;     // プレイヤーの最高速度
	float cameraOffset = 4.0f; // カメラのオフセット
	int bulletGaugeMax = 5;    // 弾のゲージ最大数
	float stunDuration = 1.0f; // スタンする時間(秒)
	float speedDownStrength = 2.0f; // スタン時の減速の強さ
	float shakeStrength = 0.5f; // スタン時の画面揺れの強さ
};