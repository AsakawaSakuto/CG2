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

	expItemGetParticle_->Initialize(&ctx_->dxCommon);
	expItemGetParticle_->LoadJson("expItemGet");
	expItemGetParticle_->Stop();
}

void CollisionManager::Update() {
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
			goResult_ = true;
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
					// 衝突した敵を死亡状態にする
					enemyDieParticle_->SetEmitterPosition(enemy->GetPosition());
					enemyDieParticle_->Play(false);

					switch (bullet->GetBulletType())
					{
					case BulletType::None:
						// 通常弾なので弾も消滅
						enemy->Dead();
						bullet->Dead();
						break;
					case BulletType::Penetration:
						// 貫通するので次の敵のチェックを続ける
						enemy->Dead();
						bullet->DecrementPenetrationCount();
						break;
					case BulletType::Bounce:
						// 反射するので次の敵のチェックを続ける
						enemy->Dead();
						bullet->DecrementBounceCount();
						break;
					}
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

	// プレイヤーの球体コライダーを取得
	const Sphere& playerSphere = player_->GetSphereCollision();
	const Sphere& stateChangeSphere = player_->GetExpItemStateChangeCollision();

	// 敵のリストを取得
	const auto& expItems = enemyManager_->GetExpItems();

	// 全ての敵をチェック
	for (const auto& expItem : expItems) {
		const Sphere& expItemSphere = expItem->GetSphereCollision();

		// プレイヤーとEnemyの衝突判定
		if (Collision::IsHit(stateChangeSphere, expItemSphere)) {
			// 衝突したEnemyを死亡状態にする
			expItem->StateChange();
		}

		// プレイヤーとEnemyの衝突判定
		if (Collision::IsHit(playerSphere, expItemSphere)) {
			// 衝突したEnemyを死亡状態にする

			expItemGetParticle_->SetEmitterPosition(player_->GetPosition());
			expItemGetParticle_->Play(false);
			player_->AddExp(5);

			expItem->Dead();
		}
	}
}