#pragma once
#include "Application/GameObject/EnemyManager/Enemy/Enemy.h"

class EnemyManager {
public:
	void Initialize(AppContext* ctx);

	void Update();

	void Draw(Camera camera);

private:
	AppContext* ctx_;
	std::vector<std::unique_ptr<Enemy>> enemies_;
	GameTimer spawnTimer_;
};