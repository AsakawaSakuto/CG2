#include "EnemyManager.h"

void EnemyManager::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	spawnTimer_.Start(2.0f, true);
}

void EnemyManager::Update() {
	if (spawnTimer_.IsFinished()) {
		auto enemy = std::make_unique<Enemy>();
		enemy->Initialize(ctx_);
		enemies_.push_back(std::move(enemy));
	}

	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	spawnTimer_.Update();
}

void EnemyManager::Draw(Camera camera) {
	for (auto& enemy : enemies_) {
		enemy->Draw(camera);
	}
}