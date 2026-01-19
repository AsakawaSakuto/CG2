#include "CollisionManager.h"
#include "GameObject/Player/Player.h"
#include "GameObject/EnemyManager/EnemyManager.h"
#include "GameObject/Player/WeaponManager/WeaponManager.h"
#include "GameObject/Player/WeaponManager/Weapon/Weapon.h"
#include "Utility/Collision/Collision.h"

void CollisionManager::Initialize() {

	enemyDieParticle_->Initialize();
	enemyDieParticle_->LoadJson("EnemyDie");
	enemyDieParticle_->Stop();

	expItemGetParticle_->Initialize();
	expItemGetParticle_->LoadJson("expItemGet");
	expItemGetParticle_->Stop();
	
	// キャッシュの予約
	aliveEnemiesCache_.reserve(aliveEnemiesCacheReserveIndex_);
}

void CollisionManager::CacheAliveEnemies() {
	aliveEnemiesCache_.clear();
	
	if (!enemyManager_) {
		return;
	}
	
	const auto& enemies = enemyManager_->GetEnemies();
	for (const auto& enemy : enemies) {
		if (enemy->IsAlive()) {
			aliveEnemiesCache_.push_back(enemy.get());
		}
	}
}

void CollisionManager::Update() {
	// 生存している敵をキャッシュ（フレームの最初に1回だけ）
	CacheAliveEnemies();
	
	// PlayerとEnemyの衝突判定を実行
	CheckPlayerEnemyCollision();

	//
	CheckExpItemPlayerCollision();

	// BulletとEnemyの衝突判定を実行
	CheckBulletEnemyCollision();

	enemyDieParticle_->Update();
	expItemGetParticle_->Update();
}

void CollisionManager::Draw(Camera camera) {
	enemyDieParticle_->Draw(camera);
	expItemGetParticle_->Draw(camera);
}

void CollisionManager::CheckPlayerEnemyCollision() {
	// PlayerまたはEnemyManagerが設定されていない場合は何もしない
	if (!player_ || !enemyManager_) {
		return;
	}

	// プレイヤーが死亡している場合は当たり判定をスキップ
	if (!player_->IsAlive()) {
		return;
	}

	// プレイヤーの球体コライダーを取得
	const Sphere& playerSphere = player_->GetSphereCollision();

	// キャッシュされた生存敵をチェック
	for (Enemy* enemy : aliveEnemiesCache_) {
		const Sphere& enemySphere = enemy->GetSphereCollision();

		// プレイヤーとEnemyの衝突判定
		if (Collision::IsHit(playerSphere, enemySphere)) {
			// 敵の攻撃力分だけプレイヤーにダメージを与える
			int enemyPower = enemy->GetPower();
			player_->TakeDamage(enemyPower);
		}
	}
}

// 距離の二乗を計算するヘルパー関数
inline float DistanceSquared(const Vector3& a, const Vector3& b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return dx * dx + dy * dy + dz * dz;
}

void CollisionManager::CheckBulletEnemyCollision() {
	// WeaponManagerまたはEnemyManagerが設定されていない場合は何もしない
	if (!weaponManager_ || !enemyManager_) {
		return;
	}

	// プレイヤーが死亡している場合は当たり判定をスキップ
	if (!player_ || !player_->IsAlive()) {
		return;
	}

	// 生存敵がいなければスキップ
	if (aliveEnemiesCache_.empty()) {
		return;
	}

	// 全ての武器をチェック
	const auto& weapons = weaponManager_->GetWeapons();
	for (const auto& weapon : weapons) {
		CheckFireBallCollision(weapon.get());
		CheckLaserCollision(weapon.get());
		CheckRunaCollision(weapon.get());
		CheckAxeCollision(weapon.get());
		CheckBoomerangCollision(weapon.get());
		CheckDiceCollision(weapon.get());
		CheckToxicCollision(weapon.get());
		CheckAreaCollision(weapon.get());
		CheckGunCollision(weapon.get());
	}
}

void CollisionManager::ApplyDamageToEnemy(Enemy* enemy, int damage, const Vector3& knockbackDir, float knockbackPower, WeaponName weaponName) {
	int enemyHPBefore = enemy->GetHP();
	enemy->Damage(damage);
	enemy->ApplyKnockback(knockbackDir, knockbackPower);
	
	// 敵が倒されたかチェック
	if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
		weaponManager_->IncrementWeaponKillCount(weaponName);
	}
	
	enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
}

void CollisionManager::CheckFireBallCollision(Weapon* weapon) {
	const auto& fireBalls = weapon->GetFireBalls();
	
	for (const auto& bullet : fireBalls) {
		if (!bullet->IsAlive()) {
			continue;
		}

		const Sphere& bulletSphere = bullet->GetSphereCollision();
		const Vector3& bulletPos = bulletSphere.center;
		float bulletRadiusSq = bulletSphere.radius * bulletSphere.radius;

		for (Enemy* enemy : aliveEnemiesCache_) {
			if (!enemy->IsAlive()) continue;
			
			const Sphere& enemySphere = enemy->GetSphereCollision();
			
			// 距離の二乗で早期棄却
			float maxDist = bulletSphere.radius + enemySphere.radius;
			float distSq = DistanceSquared(bulletPos, enemySphere.center);
			if (distSq > maxDist * maxDist) {
				continue;
			}

			if (!enemy->IsActiveInvincibleTimer()) {
				enemyDieParticle_->Play(enemy->GetPosition(), false);

				int damage = static_cast<int>(bullet->GetDamage() * player_->GetDamageRate());
				Vector3 knockbackDir = (enemySphere.center - bulletPos).Normalized();
				ApplyDamageToEnemy(enemy, damage, knockbackDir, knockbackPower_, bullet->GetWeaponName());
				
				bullet->Dead();
				break; // FireBallは1体にしか当たらない
			}
		}
	}
}

void CollisionManager::CheckLaserCollision(Weapon* weapon) {
	const auto& lasers = weapon->GetLaser();
	
	for (const auto& bullet : lasers) {
		if (!bullet->IsAlive()) {
			continue;
		}

		const Sphere& bulletSphere = bullet->GetSphereCollision();
		const Vector3& bulletPos = bulletSphere.center;

		for (Enemy* enemy : aliveEnemiesCache_) {
			if (!enemy->IsAlive()) continue;
			
			if (bullet->HasHitEnemy(enemy)) {
				continue;
			}

			const Sphere& enemySphere = enemy->GetSphereCollision();
			
			// 距離の二乗で早期棄却
			float maxDist = bulletSphere.radius + enemySphere.radius;
			float distSq = DistanceSquared(bulletPos, enemySphere.center);
			if (distSq > maxDist * maxDist) {
				continue;
			}

			if (!enemy->IsActiveInvincibleTimer()) {
				enemyDieParticle_->Play(enemy->GetPosition(), false);

				int damage = static_cast<int>(bullet->GetDamage() * player_->GetDamageRate());
				Vector3 knockbackDir = (enemySphere.center - bulletPos).Normalized();
				ApplyDamageToEnemy(enemy, damage, knockbackDir, 3.0f, bullet->GetWeaponName());
				
				bullet->MarkEnemyAsHit(enemy);
				
				if (bullet->GetPenetrationCount() > 0) {
					bullet->DecrementPenetrationCount();
				}
			}
		}
	}
}

void CollisionManager::CheckRunaCollision(Weapon* weapon) {
	const auto& runas = weapon->GetRuna();
	
	for (const auto& bullet : runas) {
		if (!bullet->IsAlive()) {
			continue;
		}

		const Sphere& bulletSphere = bullet->GetSphereCollision();
		const Vector3& bulletPos = bulletSphere.center;

		for (Enemy* enemy : aliveEnemiesCache_) {
			if (!enemy->IsAlive()) continue;
			
			if (bullet->HasHitEnemy(enemy)) {
				continue;
			}

			const Sphere& enemySphere = enemy->GetSphereCollision();
			
			// 距離の二乗で早期棄却
			float maxDist = bulletSphere.radius + enemySphere.radius;
			float distSq = DistanceSquared(bulletPos, enemySphere.center);
			if (distSq > maxDist * maxDist) {
				continue;
			}

			if (!enemy->IsActiveInvincibleTimer()) {
				enemyDieParticle_->Play(enemy->GetPosition(), false);

				int damage = static_cast<int>(bullet->GetDamage() * player_->GetDamageRate());
				Vector3 knockbackDir = (enemySphere.center - bulletPos).Normalized();
				ApplyDamageToEnemy(enemy, damage, knockbackDir, 4.0f, bullet->GetWeaponName());
				
				bullet->MarkEnemyAsHit(enemy);
				bullet->Bounce();
			}
		}
	}
}

void CollisionManager::CheckAxeCollision(Weapon* weapon) {
	const auto& axes = weapon->GetAxe();
	
	for (const auto& bullet : axes) {
		if (!bullet->IsAlive()) {
			continue;
		}

		const Sphere& bulletSphere = bullet->GetSphereCollision();
		const Vector3& bulletPos = bulletSphere.center;

		for (Enemy* enemy : aliveEnemiesCache_) {
			if (!enemy->IsAlive()) continue;
			
			const Sphere& enemySphere = enemy->GetSphereCollision();
			
			// 距離の二乗で早期棄却
			float maxDist = bulletSphere.radius + enemySphere.radius;
			float distSq = DistanceSquared(bulletPos, enemySphere.center);
			if (distSq > maxDist * maxDist) {
				continue;
			}

			if (!enemy->IsActiveInvincibleTimer()) {
				enemyDieParticle_->Play(enemy->GetPosition(), false);

				int damage = static_cast<int>(bullet->GetDamage() * player_->GetDamageRate());
				Vector3 knockbackDir = (enemySphere.center - bulletPos).Normalized();
				ApplyDamageToEnemy(enemy, damage, knockbackDir, 4.5f, bullet->GetWeaponName());
			}
		}
	}
}

void CollisionManager::CheckBoomerangCollision(Weapon* weapon) {
	const auto& boomerangs = weapon->GetBoomerang();
	
	for (const auto& bullet : boomerangs) {
		if (!bullet->IsAlive()) {
			continue;
		}

		const Sphere& bulletSphere = bullet->GetSphereCollision();
		const Vector3& bulletPos = bulletSphere.center;

		for (Enemy* enemy : aliveEnemiesCache_) {
			if (!enemy->IsAlive()) continue;
			
			const Sphere& enemySphere = enemy->GetSphereCollision();
			
			// 距離の二乗で早期棄却
			float maxDist = bulletSphere.radius + enemySphere.radius;
			float distSq = DistanceSquared(bulletPos, enemySphere.center);
			if (distSq > maxDist * maxDist) {
				continue;
			}

			if (!enemy->IsActiveInvincibleTimer()) {
				enemyDieParticle_->Play(enemy->GetPosition(), false);

				int damage = static_cast<int>(bullet->GetDamage() * player_->GetDamageRate());
				Vector3 knockbackDir = (enemySphere.center - bulletPos).Normalized();
				ApplyDamageToEnemy(enemy, damage, knockbackDir, 3.5f, bullet->GetWeaponName());
			}
		}
	}
}

void CollisionManager::CheckDiceCollision(Weapon* weapon) {
	const auto& dices = weapon->GetDice();
	
	for (const auto& bullet : dices) {
		if (!bullet->IsAlive()) {
			continue;
		}

		const Sphere& bulletSphere = bullet->GetSphereCollision();
		const Vector3& bulletPos = bulletSphere.center;

		for (Enemy* enemy : aliveEnemiesCache_) {
			if (!enemy->IsAlive()) continue;
			
			const Sphere& enemySphere = enemy->GetSphereCollision();
			
			// 距離の二乗で早期棄却
			float maxDist = bulletSphere.radius + enemySphere.radius;
			float distSq = DistanceSquared(bulletPos, enemySphere.center);
			if (distSq > maxDist * maxDist) {
				continue;
			}

			if (!enemy->IsActiveInvincibleTimer()) {
				enemyDieParticle_->Play(enemy->GetPosition(), false);

				int damage = static_cast<int>(bullet->GetRandDamage() * player_->GetDamageRate());
				Vector3 knockbackDir = (enemySphere.center - bulletPos).Normalized();
				ApplyDamageToEnemy(enemy, damage, knockbackDir, 6.0f, bullet->GetWeaponName());
				
				bullet->Dead();
			}
		}
	}
}

void CollisionManager::CheckToxicCollision(Weapon* weapon) {
	const auto& toxics = weapon->GetToxic();
	
	for (const auto& bullet : toxics) {
		if (!bullet->IsAlive()) {
			continue;
		}

		const Sphere& bulletSphere = bullet->GetSphereCollision();
		const Vector3& bulletPos = bulletSphere.center;

		for (Enemy* enemy : aliveEnemiesCache_) {
			if (!enemy->IsAlive()) continue;
			
			const Sphere& enemySphere = enemy->GetSphereCollision();
			
			// 距離の二乗で早期棄却
			float maxDist = bulletSphere.radius + enemySphere.radius;
			float distSq = DistanceSquared(bulletPos, enemySphere.center);
			if (distSq > maxDist * maxDist) {
				continue;
			}

			if (!enemy->IsActiveInvincibleTimer()) {
				enemyDieParticle_->Play(enemy->GetPosition(), false);

				int damage = static_cast<int>(bullet->GetDamage() * player_->GetDamageRate());
				Vector3 knockbackDir = (enemySphere.center - bulletPos).Normalized();
				ApplyDamageToEnemy(enemy, damage, knockbackDir, 2.5f, bullet->GetWeaponName());
			}
		}
	}
}

void CollisionManager::CheckAreaCollision(Weapon* weapon) {
	const auto& area = weapon->GetArea();
	
	if (!area || !area->IsAlive()) {
		return;
	}

	const Sphere& areaSphere = area->GetSphereCollision();
	const Vector3& areaPos = areaSphere.center;

	for (Enemy* enemy : aliveEnemiesCache_) {
		if (!enemy->IsAlive()) continue;
		
		const Sphere& enemySphere = enemy->GetSphereCollision();
		
		// 距離の二乗で早期棄却
		float maxDist = areaSphere.radius + enemySphere.radius;
		float distSq = DistanceSquared(areaPos, enemySphere.center);
		if (distSq > maxDist * maxDist) {
			continue;
		}

		if (!enemy->IsActiveInvincibleTimer()) {
			int damage = static_cast<int>(area->GetDamage() * player_->GetDamageRate());
			Vector3 knockbackDir = (enemySphere.center - areaPos).Normalized();
			ApplyDamageToEnemy(enemy, damage, knockbackDir, 2.0f, area->GetWeaponName());
		}
	}
}

void CollisionManager::CheckGunCollision(Weapon* weapon) {
	const auto& guns = weapon->GetGun();
	
	for (const auto& bullet : guns) {
		if (!bullet->IsAlive()) {
			continue;
		}

		const Sphere& bulletSphere = bullet->GetSphereCollision();
		const Vector3& bulletPos = bulletSphere.center;

		for (Enemy* enemy : aliveEnemiesCache_) {
			if (!enemy->IsAlive()) continue;
			
			const Sphere& enemySphere = enemy->GetSphereCollision();
			
			// 距離の二乗で早期棄却
			float maxDist = bulletSphere.radius + enemySphere.radius;
			float distSq = DistanceSquared(bulletPos, enemySphere.center);
			if (distSq > maxDist * maxDist) {
				continue;
			}

			if (!enemy->IsActiveInvincibleTimer()) {
				enemyDieParticle_->Play(enemy->GetPosition(), false);

				int damage = static_cast<int>(bullet->GetDamage() * player_->GetDamageRate());
				Vector3 knockbackDir = (enemySphere.center - bulletPos).Normalized();
				ApplyDamageToEnemy(enemy, damage, knockbackDir, 7.0f, bullet->GetWeaponName());

				bullet->Dead();
				break; // Gunは1体にしか当たらない
			}
		}
	}
}

void CollisionManager::CheckExpItemPlayerCollision() {
	// PlayerまたはEnemyManagerが設定されていない場合は何もしない
	if (!player_ || !enemyManager_) {
		return;
	}

	// プレイヤーが死亡している場合は経験値アイテムの当たり判定をスキップ
	if (!player_->IsAlive()) {
		return;
	}

	// プレイヤーの球体コライダーを取得
	const Sphere& playerSphere = player_->GetSphereCollision();
	const Sphere& stateChangeSphere = player_->GetExpItemStateChangeCollision();
	const Vector3& playerPos = playerSphere.center;

	// 敵のリストを取得
	const auto& expItems = enemyManager_->GetExpItems();

	// 全てのExpItemをチェック
	for (const auto& expItem : expItems) {
		if (!expItem->IsAlive()) {
			continue;
		}
		
		const Sphere& expItemSphere = expItem->GetSphereCollision();
		const Vector3& itemPos = expItemSphere.center;
		
		// StateChange用の距離チェック
		float stateChangeMaxDist = stateChangeSphere.radius + expItemSphere.radius;
		float distSq = DistanceSquared(playerPos, itemPos);
		
		if (distSq <= stateChangeMaxDist * stateChangeMaxDist) {
			expItem->StateChange();
		}

		// アイテム取得用の距離チェック
		float getMaxDist = playerSphere.radius + expItemSphere.radius;
		if (distSq <= getMaxDist * getMaxDist) {
			MyAudio::Play(SE_List::ExpGet);
			expItemGetParticle_->Play(player_->GetPosition(), false);
			player_->AddExp(MyRand::Int(3, 7));
			expItem->Dead();
		}
	}
}