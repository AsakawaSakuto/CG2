#pragma once
#include "Data/PaidChest.h"
#include "Data/FreeChest.h"
#include "Map/Map3D.h"
#include "Camera/Camera.h"
#include <vector>
#include <memory>

class JarManager; // 前方宣言

class ChestManager
{
public:
	void Initialize(Map3D* map, JarManager* jarManager = nullptr);
	void Update();
	void Draw(Camera& camera);
	void DrawImGui();
	
	// 宝箱を開ける（戻り値: 開けることができたか）
	bool OpenChest(const AABB& interactAABB, bool& outIsPaidChest);
	
private:
	// 最上面のNormalブロックを見つける
	bool FindTopNormalBlock(Map3D* map, uint32_t x, uint32_t z, uint32_t& outY) const;
	
	// 他の宝箱や壺と十分離れているかチェック
	bool IsPositionValid(const Vector3& pos, float minDistance, const std::vector<Vector3>& existingPositions) const;
	
	// 宝箱を生成する
	void SpawnChests(Map3D* map, const std::vector<Vector3>& jarPositions);
	
private:
	int paidChestNum_ = 45;
	int freeChestNum_ = 5;
	
	std::vector<std::unique_ptr<PaidChest>> paidChests_;
	std::vector<std::unique_ptr<FreeChest>> freeChests_;
	
	// 宝箱同士の最小距離
	float minChestDistance_ = 3.0f;
	// 宝箱と壺の最小距離
	float minChestJarDistance_ = 2.5f;
};