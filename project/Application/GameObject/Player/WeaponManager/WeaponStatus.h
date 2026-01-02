#pragma once
#include "Utility/GameTimer/GameTimer.h"

enum class WeaponName {
	FireBall,   // ファイヤーボール
	Laser,      // レーザー
	Runa,       // ルナ
	Axe,        // アックス
};

/// <summary>
/// プレイヤーが装備する武器のステータス構造体
/// </summary>
struct WeaponStatus {
	float cooldownTime;      // 複数打つ際のクールタイム
	float intervalTime;      // 攻撃間隔
	int shotMaxCount;        // 撃つ個数
	int shotNowCount;        // 現在撃った個数
	float damage;            // ダメージ
	int criticalRand;        // クリティカル発生確率
	float curiticalDamage;   // 武器のクリティカルダメージ率
	float moveSpeed;         // 武器の速度
	int bounceCount;         // 跳弾する回数
	int penetrationCount;    // 貫通する回数
	float nockBackPower;     // ノックバックさせる力
	float durationTime;      // 持続時間
	float lifeTime;          // 弾の寿命
};