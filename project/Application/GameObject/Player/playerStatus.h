#pragma once
#include "Math/Type/Vector3.h"

enum class PlayerName {
	PowerMan,
	TankMan,
	JumpMan,
	SpeedMan,

	Count
};

struct PlayerStatus {
	// HP関連
	int maxHP = 100;         // 最大体力
	int currentHP = 100;     // 現在の体力

	// 経験値関連
	int currentExp = 0;       // 現在の経験値
	int expToNextLevel = 100; // 次のレベルまでに必要な経験値
	int level = 1;            // 現在のレベル
	int nowMoney = 0;         // 所持金

	float moneyMultiply = 1.0f; // 所持金倍率
	float expMultiply = 1.0f;   // 経験値倍率
	// シールド値
	// アーマー値
	// ライフ吸収値
	// 運
	// 難易度
	// .XP回収範囲
	int killEnemyCount = 0;   // 敵を倒した回数
	float moveSpeed = 7.5f;   // 移動速度
	int jumpCanCount = 1;     // ジャンプ可能回数
	int currentJumpCount = 0; // 現在のジャンプ回数
	float jumpPower = 15.0f;  // ジャンプ力
	float velocity_Y = 0.0f;  // Y軸方向の速度
	float gravity = 20.0f;    // 重力
	float damageRate = 1.0f;  // ダメージ倍率
};