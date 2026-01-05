#pragma once
#include "Utility/GameTimer/GameTimer.h"

enum class WeaponName {
	None,      // 武器なし
	FireBall,  // ファイヤーボール
	Laser,     // レーザー
	Runa,      // ルナ
	Axe,       // アックス
	Boomerang, // ブーメラン
	Dice,      // ダイス
	Toxic,     // トキシック
	Area,      // エリア
	Gun,       // ガン

	Count
};

/// <summary>
/// プレイヤーが装備する武器のステータス構造体
/// </summary>
struct WeaponStatus {
	float cooldownTime = 0.2f;    // 複数打つ際のクールタイム
	float intervalTime = 1.0f;    // 攻撃間隔
	float shotMaxCount = 1.0f;    // 撃つ個数
	int shotNowCount = 0;         // 現在撃った個数
	float damage = 10.0f;         // ダメージ
	float criticalRand = 0.0f;    // クリティカル発生確率
	float curiticalDamage = 2.0f; // 武器のクリティカルダメージ率
	float speedRate = 1.0f;       // 武器の速度
	float bounceCount = 0;        // 跳弾する回数
	float penetrationCount = 0;   // 貫通する回数
	float nockBackPower = 0.0f;   // ノックバックさせる力
	float lifeTime = 1.0f;        // 弾の持続時間
	float sizeRate = 1.0f;        // 弾の大きさ
	bool useRandomTarget = false; // ランダムターゲット選択を使用するか
};