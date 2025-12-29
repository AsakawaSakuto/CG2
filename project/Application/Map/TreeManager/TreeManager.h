#pragma once
#include "Data/Tree.h"
#include "Map/Map3D.h"
#include "Camera/Camera.h"
#include <vector>
#include <memory>

class TreeManager
{
public:
	void Initialize(Map3D* map);
	void Update();
	void Draw(Camera& camera);
	void DrawImGui();
	
	// 全ての木の位置を取得（他のマネージャーで使用）
	std::vector<Vector3> GetAllTreePositions() const;
	
	// PlayerとTreeのXZ軸衝突解決（Y軸は無視）
	void ResolvePlayerCollision(Vector3& playerPosition, const AABB& playerAABB);
	
private:
	// 最上面のNormalブロックを見つける
	bool FindTopNormalBlock(Map3D* map, uint32_t x, uint32_t z, uint32_t& outY) const;
	
	// 他の木と十分離れているかチェック
	bool IsPositionValid(const Vector3& pos, float minDistance) const;
	
	// 木を生成する
	void SpawnTrees(Map3D* map);
	
private:
	int treeNum_ = 150;
	
	std::vector<std::unique_ptr<Tree>> trees_;
	
	// 木同士の最小距離
	float minTreeDistance_ = 5.0f;
};