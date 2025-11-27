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
        // centerを基準にワールド座標のminとmaxを計算
        Vector3 aMin = { a.center.x + a.min.x, a.center.y + a.min.y, a.center.z + a.min.z };
        Vector3 aMax = { a.center.x + a.max.x, a.center.y + a.max.y, a.center.z + a.max.z };
        Vector3 bMin = { b.center.x + b.min.x, b.center.y + b.min.y, b.center.z + b.min.z };
        Vector3 bMax = { b.center.x + b.max.x, b.center.y + b.max.y, b.center.z + b.max.z };
        
        // 各軸で分離していなければ衝突
        return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
               (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
               (aMin.z <= bMax.z && aMax.z >= bMin.z);
    }

    /// <summary>
    /// Sphere × AABB 判定（最近点との距離で判定）
    /// </summary>
    inline bool IsHit(const Sphere& sphere, const AABB& box)
    {
        // centerを基準にワールド座標のminとmaxを計算
        Vector3 boxMin = { box.center.x + box.min.x, box.center.y + box.min.y, box.center.z + box.min.z };
        Vector3 boxMax = { box.center.x + box.max.x, box.center.y + box.max.y, box.center.z + box.max.z };
        
        // AABB内でSphere.centerに最も近い点を求める
        Vector3 closestPoint;
        closestPoint.x = std::clamp(sphere.center.x, boxMin.x, boxMax.x);
        closestPoint.y = std::clamp(sphere.center.y, boxMin.y, boxMax.y);
        closestPoint.z = std::clamp(sphere.center.z, boxMin.z, boxMax.z);

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
