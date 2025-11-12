#include "EnemyManager.h"
#include "Engine/System/Utility/Collision/Collision.h"

void EnemyManager::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	spawnTimer_.Start(0.01f, true);
}

void EnemyManager::Update() {
	if (enemies_.size() < 350) {
		spawnTimer_.Update();

		if (spawnTimer_.IsFinished()) {
			auto enemy = std::make_unique<Enemy>();
			enemy->Initialize(ctx_);
			enemies_.push_back(std::move(enemy));
		}
	}

	// Enemy同士の衝突判定と押し出し処理
	for (size_t i = 0; i < enemies_.size(); ++i) {
		for (size_t j = i + 1; j < enemies_.size(); ++j) {
			const Sphere& sphere1 = enemies_[i]->GetSphereCollision();
			const Sphere& sphere2 = enemies_[j]->GetSphereCollision();

			// 衝突判定
			if (Collision::IsHit(sphere1, sphere2)) {
				// お互いを押し出す
				enemies_[i]->PushAway(sphere2.center, sphere2.radius);
				enemies_[j]->PushAway(sphere1.center, sphere1.radius);
			}
		}
	}

	for (auto& enemy : enemies_) {
		enemy->Update();
	}
}

void EnemyManager::Draw(Camera camera) {
	for (auto& enemy : enemies_) {
		enemy->Draw(camera);
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
		enemy->Initialize(ctx_);
		enemies_.push_back(std::move(enemy));
	}
	
	if (ImGui::Button("Clear All Enemies")) {
		enemies_.clear();
	}
	
	ImGui::End();
#endif
}

void EnemyManager::SetTargetPosition(const Vector3& target) {
	for (auto& enemy : enemies_) {
		enemy->SetTargetPosition(target);
	}
}