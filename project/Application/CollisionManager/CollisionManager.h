#pragma once
#include "EngineSystem.h"
#include <vector>

// 前方宣言
class Player;
class EnemyManager;
class WeaponManager;
class Enemy;

class CollisionManager
{
public:
	
	void Initialize();

	void Update();

	void Draw(Camera camera);

	// PlayerとEnemyManagerへの参照を設定
	void SetPlayer(Player* player) { player_ = player; }
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	void SetWeaponManager(WeaponManager* weaponManager) { weaponManager_ = weaponManager; }

	bool GetGoResult() const { return goResult_; }
private:
	// PlayerとEnemyの衝突判定
	void CheckPlayerEnemyCollision();

	//
	void CheckExpItemPlayerCollision();

	// BulletとEnemyの衝突判定
	void CheckBulletEnemyCollision();
	
	// 生存している敵のリストをキャッシュ
	void CacheAliveEnemies();

private:

	bool goResult_ = false;

	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	WeaponManager* weaponManager_ = nullptr;

	unique_ptr<Particles> enemyDieParticle_ = make_unique<Particles>();
	unique_ptr<Particles> expItemGetParticle_ = make_unique<Particles>();
	
	// 生存している敵のキャッシュ（毎フレーム更新)
	std::vector<Enemy*> aliveEnemiesCache_;
};