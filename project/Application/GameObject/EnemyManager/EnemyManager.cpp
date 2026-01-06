#include <algorithm>
#include <unordered_map>
#include "EnemyManager.h"
#include "Utility/Collision/Collision.h"
#include "Map/Map3D.h"
#include "GameObject/Player/Player.h"

void EnemyManager::Initialize() {
	spawnTimer_.Start(1.0f, true);

	dieParticle_ = std::make_unique<Particles>();
	dieParticle_->Initialize();
	dieParticle_->LoadJson("EnemyDie");
	
	// ベクターの予約でメモリ再割り当てを減らす
	enemies_.reserve(300);
	expItems_.reserve(300);
	damagePlanes_.reserve(300);

	// 初期スポーン設定
	spawnConfig_ = EnemySpawnConfig::CalculateFromPlayerLevel(1);
	currentPlayerLevel_ = 1;
}

void EnemyManager::UpdateSpawnConfig(int playerLevel) {
	if (currentPlayerLevel_ != playerLevel) {
		currentPlayerLevel_ = playerLevel;
		spawnConfig_ = EnemySpawnConfig::CalculateFromPlayerLevel(playerLevel);
		
		// スポーン間隔を更新
		spawnTimer_.Start(spawnConfig_.spawnInterval, true);
	}
}

void EnemyManager::Update() {
	// プレイヤーレベルを取得してスポーン設定を更新
	if (player_) {
		UpdateSpawnConfig(player_->GetLevel());
	}

	if (enemies_.size() < static_cast<size_t>(spawnConfig_.maxEnemyCount)) {
		spawnTimer_.Update();

		if (spawnTimer_.IsFinished()) {
			// 1回のスポーンで複数体生成（レベルに応じて増加）
			for (int spawnIndex = 0; spawnIndex < spawnConfig_.enemiesPerSpawn; ++spawnIndex) {
				auto enemy = std::make_unique<Enemy>();
				enemy->Initialize();

				// 敵のステータスをプレイヤーレベルに応じてスケーリング
				enemy->ApplyStatusMultipliers(
					spawnConfig_.hpMultiplier,
					spawnConfig_.powerMultiplier,
					spawnConfig_.moveSpeedMultiplier
				);
				
				// Map3Dを設定
				if (map_) {
					enemy->SetMap(map_);
				}
				
				// 死亡時のコールバックを設定（Playerのキルカウントをインクリメント）
				if (player_) {
					// ラムダキャプチャでspawnConfigのコピーを取得（コールバック時点での値を使用）
					float moneyMul = spawnConfig_.moneyMultiplier;
					enemy->SetOnDeathCallback([this, moneyMul]() {
						player_->IncrementKillEnemyCount();
						// レベルに応じた報酬
						int baseMoney = 1;
						int scaledMoney = static_cast<int>(baseMoney * moneyMul);
						player_->AddMoney(scaledMoney);
					});
				}

				int i = random_.Int(0, 1);
				int j = random_.Int(0, 1);
				float x = (i == 0) ? 25.0f : -25.0f;
				float z = (j == 0) ? 25.0f : -25.0f;
				
				enemy->SetPosition(
					{targetPosition_.x + random_.Float(-5.0f,5.0f) + x,
					 targetPosition_.y,
					 targetPosition_.z + random_.Float(-5.0f,5.0f) + z });
				enemies_.push_back(std::move(enemy));
			}
		}
	}

	// 空間ハッシュマップを使用した衝突判定の最適化
	const float cellSize = 5.0f;
	std::unordered_map<int64_t, std::vector<size_t>> spatialGrid;

	for (size_t i = 0; i < enemies_.size(); ++i) {
		const Vector3& pos = enemies_[i]->GetPosition();
		int32_t cellX = static_cast<int32_t>(std::floor(pos.x / cellSize));
		int32_t cellZ = static_cast<int32_t>(std::floor(pos.z / cellSize));
		int64_t cellKey = (static_cast<int64_t>(cellX) << 32) | static_cast<int64_t>(cellZ);
		spatialGrid[cellKey].push_back(i);
	}

	for (const auto& [cellKey, indices] : spatialGrid) {
		int32_t cellX = static_cast<int32_t>(cellKey >> 32);
		int32_t cellZ = static_cast<int32_t>(cellKey & 0xFFFFFFFF);

		for (int32_t dx = -1; dx <= 1; ++dx) {
			for (int32_t dz = -1; dz <= 1; ++dz) {
				int64_t neighborKey = (static_cast<int64_t>(cellX + dx) << 32) | static_cast<int64_t>(cellZ + dz);
				auto it = spatialGrid.find(neighborKey);
				if (it == spatialGrid.end()) continue;

				for (size_t i : indices) {
					for (size_t j : it->second) {
						if (i >= j) continue;

						const Sphere& sphere1 = enemies_[i]->GetSphereCollision();
						const Sphere& sphere2 = enemies_[j]->GetSphereCollision();

						if (Collision::IsHit(sphere1, sphere2)) {
							enemies_[i]->PushAway(sphere2.center, sphere2.radius);
							enemies_[j]->PushAway(sphere1.center, sphere1.radius);
						}
					}
				}
			}
		}
	}

	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	for (auto& expItem : expItems_) {
		expItem->Update();
	}

	for (auto& damagePlane : damagePlanes_) {
		damagePlane->Update();
	}

	for (auto& enemy : enemies_) {
		if (!enemy->IsAlive()) {
			dieParticle_->Play(enemy->GetPosition(), false);

			auto expItem = std::make_unique<ExpItem>();
			expItem->Initialize();
			expItem->SetPosition(enemy->GetPosition());
			expItems_.push_back(std::move(expItem));
		}
	}
	
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::unique_ptr<Enemy>& enemy) { return !enemy->IsAlive(); }),
		enemies_.end());

	expItems_.erase(
		std::remove_if(expItems_.begin(), expItems_.end(),
			[](const std::unique_ptr<ExpItem>& expItem) { return !expItem->IsAlive(); }),
		expItems_.end());

	damagePlanes_.erase(
		std::remove_if(damagePlanes_.begin(), damagePlanes_.end(),
			[](const std::unique_ptr<DamagePlane>& damagePlane) { return !damagePlane->IsAlive(); }),
		damagePlanes_.end());

	dieParticle_->Update();
}

void EnemyManager::Draw(Camera camera) {
	for (auto& enemy : enemies_) {
		enemy->Draw(camera);
	}

	for (auto& expItem : expItems_) {
		expItem->Draw(camera);
	}

	for (auto& damagePlane : damagePlanes_) {
		damagePlane->Draw(camera);
	}
}

void EnemyManager::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("EnemyManager");
	
	ImGui::Text("Enemy Count: %zu", enemies_.size());
	ImGui::Text("Damage Plane Count: %zu", damagePlanes_.size());
	ImGui::Text("Timer Active: %s", spawnTimer_.IsActive() ? "Yes" : "No");
	ImGui::Text("Timer Finished: %s", spawnTimer_.IsFinished() ? "Yes" : "No");
	ImGui::Text("Timer Progress: %.2f", spawnTimer_.GetProgress());
	ImGui::Text("Timer Elapsed: %.2f / %.2f", spawnTimer_.GetElapsedTime(), spawnTimer_.GetDuration());
	
	ImGui::Separator();
	ImGui::Text("--- Spawn Config (Level %d) ---", currentPlayerLevel_);
	ImGui::Text("HP Multiplier: %.2f", spawnConfig_.hpMultiplier);
	ImGui::Text("Power Multiplier: %.2f", spawnConfig_.powerMultiplier);
	ImGui::Text("Speed Multiplier: %.2f", spawnConfig_.moveSpeedMultiplier);
	ImGui::Text("Spawn Interval: %.2f sec", spawnConfig_.spawnInterval);
	ImGui::Text("Enemies Per Spawn: %d", spawnConfig_.enemiesPerSpawn);
	ImGui::Text("Exp Multiplier: %.2f", spawnConfig_.expMultiplier);
	ImGui::Text("Money Multiplier: %.2f", spawnConfig_.moneyMultiplier);
	
	ImGui::Separator();
	
	if (ImGui::Button("Spawn Enemy Now")) {
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize();
		enemy->ApplyStatusMultipliers(
			spawnConfig_.hpMultiplier,
			spawnConfig_.powerMultiplier,
			spawnConfig_.moveSpeedMultiplier
		);
		enemies_.push_back(std::move(enemy));
	}
	
	if (ImGui::Button("Clear All Enemies")) {
		enemies_.clear();
	}
	
	ImGui::End();
#endif
}

void EnemyManager::SetTargetPosition(const Vector3& target) {
	targetPosition_ = target;
	for (auto& enemy : enemies_) {
		enemy->SetTargetPosition(targetPosition_);
	}

	for (auto& expItem : expItems_) {
		expItem->SetTargetPosition(targetPosition_);
	}
}

void EnemyManager::CreateDamagePlane(const Vector3& position, int damage) {
	auto damagePlane = std::make_unique<DamagePlane>();
	damagePlane->Initialize(position, damage);
	damagePlanes_.push_back(std::move(damagePlane));
}