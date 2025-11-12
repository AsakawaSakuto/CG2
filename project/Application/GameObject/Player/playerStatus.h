#pragma once

struct PlayerStatus {
	int maxHP_ = 100;          // 最大体力
	// HPリジェネの値
	// 獲得するお金の倍率
	// 獲得するXPの倍率
	// シールド値
	// アーマー値
	// ライフ吸収値
	// 運
	// 難易度
	// .XP回収範囲
	float moveSpeed_ = 5.0f;   // 移動速度
	int jumpCanCount_ = 1;     // ジャンプ可能回数
	int currentJumpCount_ = 0; // 現在のジャンプ回数
	float jumpPower_ = 8.0f;   // ジャンプ力
	float velocity_Y_ = 0.0f;  // Y軸方向の速度
	float gravity_ = 20.0f;    // 重力
};