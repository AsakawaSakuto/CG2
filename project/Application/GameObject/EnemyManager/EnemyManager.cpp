#include <algorithm>
#include <unordered_map>
#include "EnemyManager.h"
#include "Utility/Collision/Collision.h"
#include "Map/Map3D.h"

void EnemyManager::Initialize() {
	spawnTimer_.Start(1.0f, true);

	dieParticle_ = std::make_unique<Particles>();
	dieParticle_->Initialize();
	dieParticle_->LoadJson("EnemyDie");
}

void EnemyManager::Update() {
	if (enemies_.size() < 150) {
		spawnTimer_.Update();

		if (spawnTimer_.IsFinished()) {
			auto enemy = std::make_unique<Enemy>();
			enemy->Initialize();
			
			// Map3Dを設定
			if (map_) {
				enemy->SetMap(map_);
			}

			int i = random_.Int(0, 1);
			int j = random_.Int(0, 1);
			float x = (i == 0) ? 10.0f : -10.0f;
			float z = (j == 0) ? 10.0f : -10.0f;
			
			enemy->SetPosition(
				{targetPosition_.x + random_.Float(-5.0f,5.0f) + x,
				 targetPosition_.y,
				 targetPosition_.z + random_.Float(-5.0f,5.0f) + z });
			enemies_.push_back(std::move(enemy));
		}
	}

	// 空間ハッシュマップを使用した衝突判定の最適化
	const float cellSize = 5.0f; // グリッドセルのサイズ
	std::unordered_map<int64_t, std::vector<size_t>> spatialGrid;

	// 敵を空間グリッドに登録
	for (size_t i = 0; i < enemies_.size(); ++i) {
		const Vector3& pos = enemies_[i]->GetPosition();
		int32_t cellX = static_cast<int32_t>(std::floor(pos.x / cellSize));
		int32_t cellZ = static_cast<int32_t>(std::floor(pos.z / cellSize));
		int64_t cellKey = (static_cast<int64_t>(cellX) << 32) | static_cast<int64_t>(cellZ);
		spatialGrid[cellKey].push_back(i);
	}

	// 同じセルと隣接セル内でのみ衝突判定
	for (const auto& [cellKey, indices] : spatialGrid) {
		int32_t cellX = static_cast<int32_t>(cellKey >> 32);
		int32_t cellZ = static_cast<int32_t>(cellKey & 0xFFFFFFFF);

		// 現在のセルと隣接8セルをチェック
		for (int32_t dx = -1; dx <= 1; ++dx) {
			for (int32_t dz = -1; dz <= 1; ++dz) {
				int64_t neighborKey = (static_cast<int64_t>(cellX + dx) << 32) | static_cast<int64_t>(cellZ + dz);
				auto it = spatialGrid.find(neighborKey);
				if (it == spatialGrid.end()) continue;

				for (size_t i : indices) {
					for (size_t j : it->second) {
						if (i >= j) continue; // 重複チェック回避

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

	// 死亡した敵を削除し、その位置でパーティクルを再生
	for (auto it = enemies_.begin(); it != enemies_.end();) {
		if (!(*it)->IsAlive()) {
			dieParticle_->Play((*it)->GetPosition(), false);

			auto expItem = std::make_unique<ExpItem>();
			expItem->Initialize();
			expItem->SetPosition((*it)->GetPosition());
			expItems_.push_back(std::move(expItem));

			it = enemies_.erase(it);
		} else {
			++it;
		}
	}

	for (auto it = expItems_.begin(); it != expItems_.end();) {
		if (!(*it)->IsAlive()) {
			it = expItems_.erase(it);
		} else {
			++it;
		}
	}

	dieParticle_->Update();
}

void EnemyManager::Draw(Camera camera) {
	for (auto& enemy : enemies_) {
		enemy->Draw(camera);
	}

	for (auto& expItem : expItems_) {
		expItem->Draw(camera);
	}
}

void EnemyManager::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("EnemyManager");
	
	ImGui::Text("Enemy Count: %zu", enemies_.size());
	ImGui::Text("Timer Active: %s", spawnTimer_.IsActive() ? "Yes" : "No");
	ImGui::Text("Timer Finished: %s", spawnTimer_.IsFinished() ? "Yes" : "No");
	ImGui::Text("Timer Progress: %.2f", spawnTimer_.GetProgress());
	ImGui::Text("Timer Elapsed: %.2f / %.2f", spawnTimer_.GetElapsedTime(), spawnTimer_.GetDuration());
	
	ImGui::Separator();
	
	if (ImGui::Button("Spawn Enemy Now")) {
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize();
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