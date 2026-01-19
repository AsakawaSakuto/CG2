#pragma once
#include "EngineSystem.h"
#include <vector>

// 前方宣言
class Player;
class EnemyManager;
class WeaponManager;
class Enemy;
class Weapon;

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
	
	// 各武器タイプ別の衝突判定メソッド
	void CheckFireBallCollision(Weapon* weapon);
	void CheckLaserCollision(Weapon* weapon);
	void CheckRunaCollision(Weapon* weapon);
	void CheckAxeCollision(Weapon* weapon);
	void CheckBoomerangCollision(Weapon* weapon);
	void CheckDiceCollision(Weapon* weapon);
	void CheckToxicCollision(Weapon* weapon);
	void CheckAreaCollision(Weapon* weapon);
	void CheckGunCollision(Weapon* weapon);
	
	// 生存している敵のリストをキャッシュ
	void CacheAliveEnemies();
	
	// 共通のダメージ処理ヘルパー
	void ApplyDamageToEnemy(Enemy* enemy, int damage, const Vector3& knockbackDir, float knockbackPower, WeaponName weaponName);

private:

	bool goResult_ = false;

	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	WeaponManager* weaponManager_ = nullptr;

	unique_ptr<Particles> enemyDieParticle_ = make_unique<Particles>();
	unique_ptr<Particles> expItemGetParticle_ = make_unique<Particles>();
	
	// 生存している敵のキャッシュ（毎フレーム更新)
	std::vector<Enemy*> aliveEnemiesCache_;
	int aliveEnemiesCacheReserveIndex_ = 256;

	float knockbackPower_ = 5.0f;
};