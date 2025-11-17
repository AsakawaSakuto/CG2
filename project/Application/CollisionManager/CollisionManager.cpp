#include "CollisionManager.h"
#include "Application/GameObject/Player/Player.h"
#include "Application/GameObject/EnemyManager/EnemyManager.h"
#include "Application/GameObject/Player/WeaponManager/WeaponManager.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Weapon.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Bullet/Bullet.h"
#include "Engine/System/Utility/Collision/Collision.h"

void CollisionManager::Initialize() {

}

void CollisionManager::Update() {
	// PlayerとEnemyの衝突判定を実行
	CheckPlayerEnemyCollision();

	// BulletとEnemyの衝突判定を実行
	CheckBulletEnemyCollision();
}

void CollisionManager::CheckPlayerEnemyCollision() {
	// PlayerまたはEnemyManagerが設定されていない場合は何もしない
	if (!player_ || !enemyManager_) {
		return;
	}

	// 衝突判定と削除処理を実行
	HandlePlayerEnemyCollision();
}

void CollisionManager::HandlePlayerEnemyCollision() {
	// プレイヤーの球体コライダーを取得
	const Sphere& playerSphere = player_->GetSphereCollision();

	// 敵のリストを取得（非const参照が必要なため、EnemyManagerに削除機能を持たせる代わりに直接アクセス）
	auto& enemies = const_cast<std::vector<std::unique_ptr<Enemy>>&>(enemyManager_->GetEnemies());

	// イテレータを使って削除処理を行う
	auto it = enemies.begin();
	while (it != enemies.end()) {
		const Sphere& enemySphere = (*it)->GetSphereCollision();
		
		// プレイヤーとEnemyの衝突判定
		if (Collision::IsHit(playerSphere, enemySphere)) {
			// 衝突したEnemyを削除
			it = enemies.erase(it);
		} else {
			++it;
		}
	}
}

void CollisionManager::CheckBulletEnemyCollision() {
	// WeaponManagerまたはEnemyManagerが設定されていない場合は何もしない
	if (!weaponManager_ || !enemyManager_) {
		return;
	}

	// 衝突判定と削除処理を実行
	HandleBulletEnemyCollision();
}

void CollisionManager::HandleBulletEnemyCollision() {
	// 敵のリストを取得
	auto& enemies = const_cast<std::vector<std::unique_ptr<Enemy>>&>(enemyManager_->GetEnemies());

	// 全ての武器をチェック
	const auto& weapons = weaponManager_->GetWeapons();
	for (const auto& weapon : weapons) {
		// 各武器の弾をチェック
		auto& bullets = const_cast<std::vector<std::unique_ptr<Bullet>>&>(weapon->GetBullets());

		// 弾のイテレータ
		auto bulletIt = bullets.begin();
		while (bulletIt != bullets.end()) {
			const Sphere& bulletSphere = (*bulletIt)->GetSphereCollision();
			bool bulletHit = false;

			// 全ての敵をチェック
			auto enemyIt = enemies.begin();
			while (enemyIt != enemies.end()) {
				const Sphere& enemySphere = (*enemyIt)->GetSphereCollision();

				// 弾と敵の衝突判定
				if (Collision::IsHit(bulletSphere, enemySphere)) {
					// 衝突した敵を削除
					enemyIt = enemies.erase(enemyIt);
					bulletHit = true;
					break; // この弾は当たったので次の弾へ
				} else {
					++enemyIt;
				}
			}

			// 弾が当たった場合は削除
			if (bulletHit) {
				bulletIt = bullets.erase(bulletIt);
			} else {
				++bulletIt;
			}
		}
	}
}
