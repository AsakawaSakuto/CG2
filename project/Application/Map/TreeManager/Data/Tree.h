#pragma once
#include "Math/Type/Vector3.h"
#include "Math/Type/Transform.h"
#include "Utility/Collision/Type/AABB.h"
#include "Camera/Camera.h"
#include "3d/Model/Model.h"
#include "3d/Line/MyDebugLine.h"
#include "Utility/Random/Random.h"

enum class TreeType {
	Min,
	Middle,
	Max
};

class Tree
{
public:
	void Initialize(Vector3 pos);
	void Draw(Camera camera);
	
	/// <summary>
	/// 木を半透明化
	/// </summary>
	/// <param name="alpha">透明度（0.0-1.0）</param>
	void SetTransparent(bool enable, float alpha = 0.3f);
	
	// Getter
	Vector3 GetPosition() const { return transform_.translate; }
	const AABB& GetAABBCollision() const { return aabbCollision_; }
	TreeType GetTreeType() const { return treeType_; }
	bool IsSpawned() const { return isSpawn_; }
	
private:
	std::unique_ptr<Model> model_;
	Transform transform_;
	TreeType treeType_;
	AABB aabbCollision_;
	bool isSpawn_ = false;
};