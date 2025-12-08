#pragma once

struct PlayerStatus {
	// HP関連
	int maxHP_ = 100;          // 最大体力
	int currentHP_ = 100;      // 現在の体力

	// 経験値関連
	int currentExp_ = 0;       // 現在の経験値
	int expToNextLevel_ = 100; // 次のレベルまでに必要な経験値
	int level_ = 1;            // 現在のレベル

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