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
private:
	std::unique_ptr<Model> model_;
	Transform transform_;
	TreeType treeType_;
	AABB aabbCollision_;
	bool isSpawn_ = false;
};