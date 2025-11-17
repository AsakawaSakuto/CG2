#include "CollisionManager.h"
#include "Application/GameObject/Player/Player.h"
#include "Application/GameObject/EnemyManager/EnemyManager.h"
#include "Engine/System/Utility/Collision/Collision.h"

void CollisionManager::Initialize() {

}

void CollisionManager::Update() {
	// PlayerとEnemyの衝突判定を実行
	CheckPlayerEnemyCollision();
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
