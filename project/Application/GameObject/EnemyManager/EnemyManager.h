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
	
	/// <summary>
	/// ハードモードを設定
	/// </summary>
	void SetHardMode(bool isHardMode) { 
		isHardMode_ = isHardMode; 
		if (isHardMode) {
			// ハードモード開始時にタイマーをリセット
			hardModePhase_ = 0;
			hardModeElapsedTime_ = 0.0f;
		}
	}
	
	/// <summary>
	/// ハードモード中かどうか
	/// </summary>
	bool IsHardMode() const { return isHardMode_; }
	
	/// <summary>
	/// ハードモード移行時に既存の敵を全て倒してExpItemをドロップさせる
	/// </summary>
	void KillAllEnemiesForHardMode();

	/// <summary>
	/// ハードモードの段階（0から開始し、10秒ごとに増加）
	/// </summary>
	int GetHardModePhase() const { return hardModePhase_; }
	
	/// <summary>
	/// ハードモードの経過時間を取得
	/// </summary>
	float GetHardModeElapsedTime() const { return hardModeElapsedTime_; }

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
	
	// ハードモードフラグ
	bool isHardMode_ = false;
	
	// ハードモードの段階（10秒ごとに1増加）
	int hardModePhase_ = 0;
	
	// ハードモード開始からの経過時間
	float hardModeElapsedTime_ = 0.0f;
	
	/// <summary>
	/// ハードモードの段階に応じた倍率を計算
	/// </summary>
	/// <param name="baseMultiplier">基本倍率</param>
	/// <param name="phaseIncrease">段階ごとの増加量</param>
	/// <returns>計算された倍率</returns>
	float CalculateHardModeMultiplier(float baseMultiplier, float phaseIncrease) const;
	
	/// <summary>
	/// ハードモードの段階に応じた色を計算（白から赤へ）
	/// </summary>
	/// <returns>RGB色（R, G, B）各0.0-2.0</returns>
	Vector3 CalculateHardModeColor() const;
};