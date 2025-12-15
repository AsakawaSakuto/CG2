#pragma once
#include "Math/Type/Vector3.h"

struct FrustumPlane {
	Vector3 normal;
	float distance;
};

// フラスタム構造体
struct Frustum {
	FrustumPlane planes[6]; // 6つの平面（左、右、上、下、近、遠）
};