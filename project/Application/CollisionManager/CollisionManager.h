#pragma once
#include "Application/EngineSystem.h"

// 前方宣言
class Player;
class EnemyManager;
class WeaponManager;

class CollisionManager
{
public:
	
	void Initialize(AppContext* ctx);

	void Update();

	void Draw(Camera camera);

	// PlayerとEnemyManagerへの参照を設定
	void SetPlayer(Player* player) { player_ = player; }
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	void SetWeaponManager(WeaponManager* weaponManager) { weaponManager_ = weaponManager; }

private:
	// PlayerとEnemyの衝突判定
	void CheckPlayerEnemyCollision();

	// BulletとEnemyの衝突判定
	void CheckBulletEnemyCollision();

private:
	AppContext* ctx_ = nullptr;

	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	WeaponManager* weaponManager_ = nullptr;

	unique_ptr<Particles> enemyDieParticle_ = make_unique<Particles>();
};