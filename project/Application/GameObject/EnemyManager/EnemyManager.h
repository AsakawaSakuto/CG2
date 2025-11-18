#pragma once
#include "Application/GameObject/EnemyManager/Enemy/Enemy.h"
#include "Application/GameObject/EnemyManager/ExpItem/ExpItem.h"

class EnemyManager {
public:
	void Initialize(AppContext* ctx);

	void Update();

	void Draw(Camera camera);

	void DrawImGui();

	void SetTargetPosition(const Vector3& target);

	// 敵のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }
	const std::vector<std::unique_ptr<ExpItem>>& GetExpItems() const { return expItems_; }

private:

	AppContext* ctx_;
	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::vector<std::unique_ptr<ExpItem>> expItems_;
	GameTimer spawnTimer_;
	Random random_;
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };

	unique_ptr<Particles> dieParticle_ = make_unique<Particles>();
};