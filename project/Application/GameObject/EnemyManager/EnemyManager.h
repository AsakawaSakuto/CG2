#pragma once
#include "Application/GameObject/EnemyManager/Enemy/Enemy.h"

class EnemyManager {
public:
	void Initialize(AppContext* ctx);

	void Update();

	void Draw(Camera camera);

	void DrawImGui();

	void SetTargetPosition(const Vector3& target);
	
	void CheckCollisionWithPlayer(const Sphere& playerSphere);

private:
	AppContext* ctx_;
	std::vector<std::unique_ptr<Enemy>> enemies_;
	GameTimer spawnTimer_;
};