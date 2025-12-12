#pragma once
#include "Math/Type/Vector3.h"

// 平面を表す構造体
struct Plane {
	Vector3 normal;   // 平面の法線
	float distance;   // 原点からの距離
};