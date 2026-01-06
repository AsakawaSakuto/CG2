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
	
	// 宝箱との衝突をチェック（戻り値: 衝突したか）
	// outIsPaidChest: PaidChestならtrue、FreeChestならfalse
	// outOpenAmount: 開けるのに必要な金額（PaidChestの場合のみ）
	bool CheckChestCollision(const AABB& interactAABB, bool& outIsPaidChest, int& outOpenAmount);
	
	// 宝箱を開ける（お金のチェック後に使用）
	// needMoney: PaidChestの場合true、FreeChestの場合false
	bool OpenChest(const AABB& interactAABB, bool needMoney);
	
	// 開けるのに必要な金額を取得
	int GetOpenAmount() const { return openAmount_; }
	
	// チェストをアクティブ状態に設定（プレイヤーが近づいたときなど）
	void SetChestActive(const AABB& interactAABB, bool hasEnoughMoney);
	
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
	
	int openAmount_ = 30;

	std::vector<std::unique_ptr<PaidChest>> paidChests_;
	std::vector<std::unique_ptr<FreeChest>> freeChests_;
	
	// 宝箱同士の最小距離
	float minChestDistance_ = 3.0f;
	// 宝箱と壺の最小距離
	float minChestJarDistance_ = 2.5f;

	int addOpenAmount_ = 10;
};