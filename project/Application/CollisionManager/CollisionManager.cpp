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
	aliveEnemiesCache_.reserve(256);
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
		// 各武器の弾をチェック
		const auto& fireBalls = weapon->GetFireBalls();
		const auto& lasers = weapon->GetLaser();
		const auto& runas = weapon->GetRuna();
		const auto& axes = weapon->GetAxe();
		const auto& boomerangs = weapon->GetBoomerang();
		const auto& dices = weapon->GetDice();
		const auto& toxics = weapon->GetToxic();
		const auto& area = weapon->GetArea();
		const auto& guns = weapon->GetGun();

		// FireBalls
		for (const auto& bullet : fireBalls) {
			if (!bullet->IsAlive()) {
				continue;
			}

			const Sphere& bulletSphere = bullet->GetSphereCollision();
			const Vector3& bulletPos = bulletSphere.center;
			float bulletRadiusSq = bulletSphere.radius * bulletSphere.radius;

			for (Enemy* enemy : aliveEnemiesCache_) {
				if (!enemy->IsAlive()) continue; // 他の弾で死んだ可能性
				
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(bullet->GetWeaponName());
					}
					
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
					
					bullet->Dead();
					break; // FireBallは1体にしか当たらない
				}
			}
		}

		// Lasers
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(bullet->GetWeaponName());
					}
					
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
					
					bullet->MarkEnemyAsHit(enemy);
					
					if (bullet->GetPenetrationCount() > 0) {
						bullet->DecrementPenetrationCount();
					}
				}
			}
		}

		// Runas
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(bullet->GetWeaponName());
					}
					
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
					
					bullet->MarkEnemyAsHit(enemy);
					
					bullet->Bounce();
				}
			}
		}

		// Axes
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(bullet->GetWeaponName());
					}
					
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
				}
			}
		}

		// Boomerangs
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(bullet->GetWeaponName());
					}
					
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
				}
			}
		}

		// Dices
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(bullet->GetWeaponName());
					}
					
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
					bullet->Dead();
				}
			}
		}

		// Toxics
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(bullet->GetWeaponName());
					}
					
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
				}
			}
		}

		// Area（常時存在する範囲攻撃）
		if (area && area->IsAlive()) {
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(area->GetWeaponName());
					}
					
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
				}
			}
		}

		// Guns
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
					int enemyHPBefore = enemy->GetHP(); // ダメージを与える前のHP
					enemy->Damage(damage);
					
					// 敵が倒されたかチェック
					if (enemyHPBefore > 0 && enemy->GetHP() <= 0) {
						// この武器でキルカウントをインクリメント
						weaponManager_->IncrementWeaponKillCount(bullet->GetWeaponName());
					}
	
					enemyManager_->CreateDamagePlane(enemy->GetPosition(), damage);
					
					bullet->Dead();
					break; // Gunは1体にしか当たらない
				}
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
			MyAudio::PlaySE(SE_List::ExpGet);
			expItemGetParticle_->Play(player_->GetPosition(), false);
			player_->AddExp(MyRand::Int(3, 7));
			expItem->Dead();
		}
	}
}