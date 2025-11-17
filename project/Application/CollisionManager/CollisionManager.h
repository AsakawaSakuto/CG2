#pragma once

// 前方宣言
class Player;
class EnemyManager;
class WeaponManager;

class CollisionManager
{
public:
	
	void Initialize();

	void Update();

	// PlayerとEnemyManagerへの参照を設定
	void SetPlayer(Player* player) { player_ = player; }
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	void SetWeaponManager(WeaponManager* weaponManager) { weaponManager_ = weaponManager; }

private:
	// PlayerとEnemyの衝突判定
	void CheckPlayerEnemyCollision();

	// PlayerとEnemyが衝突した際の処理（Enemyを削除）
	void HandlePlayerEnemyCollision();

	// BulletとEnemyの衝突判定
	void CheckBulletEnemyCollision();

	// BulletとEnemyが衝突した際の処理（両方を削除）
	void HandleBulletEnemyCollision();

private:
	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	WeaponManager* weaponManager_ = nullptr;
};