#pragma once
#include <variant>
#include "Utility/Collision/Type/AABB.h"
#include "Utility/Collision/Type/OBB.h"
#include "Utility/Collision/Type/Sphere.h"
#include "Utility/Collision/Type/OvalSphere.h"
#include "Utility/Collision/Type/Plane.h"
#include "Utility/Collision/Type/Segment.h"
#include "Utility/Collision/Type/LineStruct.h"
#include "Utility/Collision/Type/Circle.h"

/// <summary>
/// デバッグ描画用の形状を統一的に扱うための variant型定義
/// </summary>
using Shape = std::variant<
    AABB,
    OBB,
    Sphere,
    OvalSphere,
    Plane,
    Segment,
    Line,
    Circle
>;