#pragma once
#include "Data/MinJar.h"
#include "Data/MaxJar.h"
#include "Map/Map3D.h"
#include "Camera/Camera.h"
#include <vector>
#include <memory>

class JarManager
{
public:
	void Initialize(Map3D* map);
	void Update();
	void Draw(Camera& camera);
	void DrawImGui();
	
	// 壺を壊す（戻り値: ドロップアイテムの量）
	int BreakJar(const AABB& attackAABB, JarType& outJarType);
	
	// 全ての壺の位置を取得（ChestManagerで使用）
	std::vector<Vector3> GetAllJarPositions() const;
	
	void SetAABBCollision(const AABB& aabb) { aabbCollision_ = aabb; }
private:
	// 最上面のNormalブロックを見つける
	bool FindTopNormalBlock(Map3D* map, uint32_t x, uint32_t z, uint32_t& outY) const;
	
	// 他の壺と十分離れているかチェック
	bool IsPositionValid(const Vector3& pos, float minDistance) const;
	
	// 壺を生成する
	void SpawnJars(Map3D* map);
	
private:
	int minJarNum_ = 50;
	int maxJarNum_ = 25;
	
	std::vector<std::unique_ptr<MinJar>> minJars_;
	std::vector<std::unique_ptr<MaxJar>> maxJars_;

	AABB aabbCollision_;

	// 壺同士の最小距離
	float minJarDistance_ = 2.0f;
};