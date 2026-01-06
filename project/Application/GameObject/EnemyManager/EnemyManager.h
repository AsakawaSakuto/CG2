#pragma once
#include "GameObject/EnemyManager/Enemy/Enemy.h"
#include "GameObject/EnemyManager/ExpItem/ExpItem.h"
#include "GameObject/EnemyManager/DamagePlane.h"
#include "EnemySpawnConfig.h"

class Map3D; // 前方宣言
class Player; // 前方宣言

class EnemyManager {
public:
	void Initialize();

	void Update();

	void Draw(Camera camera);

	void DrawImGui();

	void SetTargetPosition(const Vector3& target);

	// 敵のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }
	const std::vector<std::unique_ptr<ExpItem>>& GetExpItems() const { return expItems_; }
	
	// Map3Dを設定
	void SetMap(Map3D* map) { map_ = map; }
	
	// Playerへの参照を設定
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// ダメージ表示を生成
	/// </summary>
	void CreateDamagePlane(const Vector3& position, int damage);

	/// <summary>
	/// 現在のスポーン設定を取得
	/// </summary>
	const EnemySpawnConfig& GetSpawnConfig() const { return spawnConfig_; }

private:

	/// <summary>
	/// プレイヤーレベルに応じてスポーン設定を更新
	/// </summary>
	void UpdateSpawnConfig(int playerLevel);

	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::vector<std::unique_ptr<ExpItem>> expItems_;
	std::vector<std::unique_ptr<DamagePlane>> damagePlanes_;
	GameTimer spawnTimer_;
	Random random_;
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };

	unique_ptr<Particles> dieParticle_ = make_unique<Particles>();
	
	// Map3Dへの参照
	Map3D* map_ = nullptr;
	
	// Playerへの参照
	Player* player_ = nullptr;

	// レベルに応じたスポーン設定
	EnemySpawnConfig spawnConfig_;
	int currentPlayerLevel_ = 1;
};