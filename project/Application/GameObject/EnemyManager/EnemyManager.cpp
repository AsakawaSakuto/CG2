#include "EnemyManager.h"

void EnemyManager::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	spawnTimer_.Start(2.0f, true);
}

void EnemyManager::Update() {
	spawnTimer_.Update();

	if (spawnTimer_.IsFinished()) {
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize(ctx_);
		enemies_.push_back(std::move(enemy));
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
}

void EnemyManager::SetTargetPosition(const Vector3& target) {
	for (auto& enemy : enemies_) {
		enemy->SetTargetPosition(target);
	}
}