#include <algorithm>
#include "EnemyManager.h"
#include "Utility/Collision/Collision.h"

void EnemyManager::Initialize() {
	spawnTimer_.Start(0.25f, true);

	dieParticle_ = std::make_unique<Particles>();
	dieParticle_->Initialize();
	dieParticle_->LoadJson("EnemyDie");
}

void EnemyManager::Update() {
	if (enemies_.size() < 50) {
		//spawnTimer_.Update();

		if (spawnTimer_.IsFinished()) {
			auto enemy = std::make_unique<Enemy>();
			enemy->Initialize();

			int i = random_.Int(0, 1);
			int j = random_.Int(0, 1);
			float x = 0.0f;
			float z = 0.0f;
			if (i == 0) {
				x = 10.0f;
			} else {
				x = -10.0f;
			}
			if (j == 0) {
				z = 10.0f;
			} else {
				z = -10.0f;
			}
			enemy->SetPosition(
				{targetPosition_.x + random_.Float(-5.0f,5.0f) + x,
				 0.0f,
				 targetPosition_.z + random_.Float(-5.0f,5.0f) + z });
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

	/*if (ctx_->input.TriggerKey(DIK_P)) {
		for (auto& expItem : expItems_) {
			expItem->StateChange();
		}
	}*/

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