#include "CollisionManager.h"
#include "Application/GameObject/Player/Player.h"
#include "Application/GameObject/EnemyManager/EnemyManager.h"
#include "Application/GameObject/Player/WeaponManager/WeaponManager.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Weapon.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Bullet/Bullet.h"
#include "Engine/System/Utility/Collision/Collision.h"

void CollisionManager::Initialize(AppContext* ctx) {
	ctx_ = ctx;

	enemyDieParticle_->Initialize(&ctx_->dxCommon);
	enemyDieParticle_->LoadJson("EnemyDie");
	enemyDieParticle_->Stop();
}

void CollisionManager::Update() {
	// PlayerとEnemyの衝突判定を実行
	CheckPlayerEnemyCollision();

	// BulletとEnemyの衝突判定を実行
	CheckBulletEnemyCollision();

	enemyDieParticle_->Update();
}

void CollisionManager::Draw(Camera camera) {
	enemyDieParticle_->Draw(camera);
}

void CollisionManager::CheckPlayerEnemyCollision() {
	// PlayerまたはEnemyManagerが設定されていない場合は何もしない
	if (!player_ || !enemyManager_) {
		return;
	}

	// プレイヤーの球体コライダーを取得
	const Sphere& playerSphere = player_->GetSphereCollision();

	// 敵のリストを取得
	const auto& enemies = enemyManager_->GetEnemies();

	// 全ての敵をチェック
	for (const auto& enemy : enemies) {
		const Sphere& enemySphere = enemy->GetSphereCollision();

		// プレイヤーとEnemyの衝突判定
		if (Collision::IsHit(playerSphere, enemySphere)) {
			// 衝突したEnemyを死亡状態にする
			enemy->Dead();
		}
	}
}

void CollisionManager::CheckBulletEnemyCollision() {
	// WeaponManagerまたはEnemyManagerが設定されていない場合は何もしない
	if (!weaponManager_ || !enemyManager_) {
		return;
	}

	// 敵のリストを取得
	const auto& enemies = enemyManager_->GetEnemies();

	// 全ての武器をチェック
	const auto& weapons = weaponManager_->GetWeapons();
	for (const auto& weapon : weapons) {
		// 各武器の弾をチェック
		const auto& bullets = weapon->GetBullets();

		for (const auto& bullet : bullets) {
			// 弾が既に死亡している場合はスキップ
			if (!bullet->IsAlive()) {
				continue;
			}

			const Sphere& bulletSphere = bullet->GetSphereCollision();

			// 全ての敵をチェック
			for (const auto& enemy : enemies) {
				// 敵が既に死亡している場合はスキップ
				if (!enemy->IsAlive()) {
					continue;
				}

				const Sphere& enemySphere = enemy->GetSphereCollision();

				// 弾と敵の衝突判定
				if (Collision::IsHit(bulletSphere, enemySphere)) {
					// 衝突した敵と弾を死亡状態にする

					enemyDieParticle_->SetEmitterPosition(enemy->GetPosition());
					enemyDieParticle_->Play(false);

					enemy->Dead();
					bullet->Dead();
					break; // この弾は当たったので次の弾へ
				}
			}
		}
	}
}