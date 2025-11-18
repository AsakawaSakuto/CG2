#pragma once
#include "Application/GameObject/EnemyManager/Enemy/Enemy.h"

class EnemyManager {
public:
	void Initialize(AppContext* ctx);

	void Update();

	void Draw(Camera camera);

	void DrawImGui();

	void SetTargetPosition(const Vector3& target);

	void IsDie();

	// 敵のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }

private:
	AppContext* ctx_;
	std::vector<std::unique_ptr<Enemy>> enemies_;
	GameTimer spawnTimer_;
	Random random_;
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };

	unique_ptr<Particles> dieParticle_ = make_unique<Particles>();
};