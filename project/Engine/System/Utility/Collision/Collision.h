#pragma once
#include "Vector3.h"
#include "Sphere.h"
#include "AABB.h"
#include <algorithm>
#include <cmath>

/// <summary>
/// 当たり判定ユーティリティ
/// </summary>
namespace Collision
{
    /// <summary>
    /// Sphere × Sphere 判定
    /// </summary>
    inline bool IsHit(const Sphere& a, const Sphere& b)
    {
        Vector3 diff = { a.center.x - b.center.x, a.center.y - b.center.y, a.center.z - b.center.z };
        float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
        float radiusSum = a.radius + b.radius;
        return distanceSq <= (radiusSum * radiusSum);
    }

    /// <summary>
    /// AABB × AABB 判定
    /// </summary>
    inline bool IsHit(const AABB& a, const AABB& b)
    {
        // 各軸で分離していなければ衝突
        return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
            (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
            (a.min.z <= b.max.z && a.max.z >= b.min.z);
    }

    /// <summary>
    /// Sphere × AABB 判定（最近点との距離で判定）
    /// </summary>
    inline bool IsHit(const Sphere& sphere, const AABB& box)
    {
        // AABB内でSphere.centerに最も近い点を求める
        Vector3 closestPoint;
        closestPoint.x = std::clamp(sphere.center.x, box.min.x, box.max.x);
        closestPoint.y = std::clamp(sphere.center.y, box.min.y, box.max.y);
        closestPoint.z = std::clamp(sphere.center.z, box.min.z, box.max.z);

        // 最近点との距離の2乗
        Vector3 diff = {
            sphere.center.x - closestPoint.x,
            sphere.center.y - closestPoint.y,
            sphere.center.z - closestPoint.z
        };
        float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

        return distanceSq <= (sphere.radius * sphere.radius);
    }

    /// <summary>
    /// AABB × Sphere 判定（呼び出し順反転のラッパ）
    /// </summary>
    inline bool IsHit(const AABB& a, const Sphere& b)
    {
        return IsHit(b, a);
    }
}
