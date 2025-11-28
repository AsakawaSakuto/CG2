#pragma once
#include "Vector3.h"
#include "Sphere.h"
#include "AABB.h"
#include "OBB.h"
#include "OvalSphere.h"
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
        // centerとsizeからワールド座標のminとmaxを計算
        Vector3 aMin = { a.center.x - a.size.x * 0.5f, a.center.y - a.size.y * 0.5f, a.center.z - a.size.z * 0.5f };
        Vector3 aMax = { a.center.x + a.size.x * 0.5f, a.center.y + a.size.y * 0.5f, a.center.z + a.size.z * 0.5f };
        Vector3 bMin = { b.center.x - b.size.x * 0.5f, b.center.y - b.size.y * 0.5f, b.center.z - b.size.z * 0.5f };
        Vector3 bMax = { b.center.x + b.size.x * 0.5f, b.center.y + b.size.y * 0.5f, b.center.z + b.size.z * 0.5f };
        
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
        // centerとsizeからワールド座標のminとmaxを計算
        Vector3 boxMin = { box.center.x - box.size.x * 0.5f, box.center.y - box.size.y * 0.5f, box.center.z - box.size.z * 0.5f };
        Vector3 boxMax = { box.center.x + box.size.x * 0.5f, box.center.y + box.size.y * 0.5f, box.center.z + box.size.z * 0.5f };
        
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

    /// <summary>
    /// 分離軸定理(SAT)のためのヘルパー関数
    /// </summary>
    namespace Detail
    {
        /// <summary>
        /// OBBを指定された軸に投影した際の半径を計算
        /// </summary>
        inline float GetProjectionRadius(const OBB& obb, const Vector3& axis)
        {
            return std::abs(obb.size.x * 0.5f * (obb.orientation[0] * axis)) +
                   std::abs(obb.size.y * 0.5f * (obb.orientation[1] * axis)) +
                   std::abs(obb.size.z * 0.5f * (obb.orientation[2] * axis));
        }

        /// <summary>
        /// 分離軸上での2つのOBBの投影が分離しているかチェック
        /// </summary>
        inline bool IsSeparatedOnAxis(const OBB& a, const OBB& b, const Vector3& axis)
        {
            // 軸がゼロベクトルの場合はスキップ（外積が0になる場合など）
            float axisLengthSq = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
            if (axisLengthSq < 0.0001f) return false;

            Vector3 normalizedAxis = axis.Normalize();
            
            // 中心間の距離を軸に投影
            Vector3 centerDiff = { b.center.x - a.center.x, b.center.y - a.center.y, b.center.z - a.center.z };
            float centerProjection = std::abs(centerDiff * normalizedAxis);

            // 各OBBの投影半径を計算
            float aProjection = GetProjectionRadius(a, normalizedAxis);
            float bProjection = GetProjectionRadius(b, normalizedAxis);

            // 分離している場合はtrue
            return centerProjection > (aProjection + bProjection);
        }
    }

    /// <summary>
    /// OBB × OBB 判定（分離軸定理）
    /// </summary>
    inline bool IsHit(const OBB& a, const OBB& b)
    {
        // 分離軸定理: 15個の軸でテスト
        // 1. Aの3軸
        // 2. Bの3軸
        // 3. 上記の外積9個

        // Aの3軸でテスト
        for (int i = 0; i < 3; ++i)
        {
            if (Detail::IsSeparatedOnAxis(a, b, a.orientation[i]))
                return false;
        }

        // Bの3軸でテスト
        for (int i = 0; i < 3; ++i)
        {
            if (Detail::IsSeparatedOnAxis(a, b, b.orientation[i]))
                return false;
        }

        // 外積の9軸でテスト
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                Vector3 axis = a.orientation[i] ^ b.orientation[j];
                if (Detail::IsSeparatedOnAxis(a, b, axis))
                    return false;
            }
        }

        // すべての軸で分離していない = 衝突している
        return true;
    }

    /// <summary>
    /// AABB × OBB 判定
    /// AABBをOBBとして扱い、OBB×OBB判定を使用
    /// </summary>
    inline bool IsHit(const AABB& aabb, const OBB& obb)
    {
        // AABBをOBBに変換（軸が整列した状態）
        OBB aabbAsOBB;
        aabbAsOBB.center = aabb.center;
        aabbAsOBB.size = aabb.size;
        aabbAsOBB.rotate = { 0.0f, 0.0f, 0.0f };
        aabbAsOBB.orientation[0] = { 1.0f, 0.0f, 0.0f }; // X軸
        aabbAsOBB.orientation[1] = { 0.0f, 1.0f, 0.0f }; // Y軸
        aabbAsOBB.orientation[2] = { 0.0f, 0.0f, 1.0f }; // Z軸

        return IsHit(aabbAsOBB, obb);
    }

    /// <summary>
    /// OBB × AABB 判定（呼び出し順反転のラッパ）
    /// </summary>
    inline bool IsHit(const OBB& obb, const AABB& aabb)
    {
        return IsHit(aabb, obb);
    }

    /// <summary>
    /// OBB × Sphere 判定
    /// OBBのローカル座標系での最近点を求める
    /// </summary>
    inline bool IsHit(const OBB& obb, const Sphere& sphere)
    {
        // 球の中心をOBBのローカル座標系に変換
        Vector3 centerDiff = {
            sphere.center.x - obb.center.x,
            sphere.center.y - obb.center.y,
            sphere.center.z - obb.center.z
        };

        // OBBの各軸に投影してローカル座標を求める
        Vector3 localCenter = {
            centerDiff * obb.orientation[0],
            centerDiff * obb.orientation[1],
            centerDiff * obb.orientation[2]
        };

        // OBBのローカル空間での半サイズ
        Vector3 halfSize = {
            obb.size.x * 0.5f,
            obb.size.y * 0.5f,
            obb.size.z * 0.5f
        };

        // ローカル空間での最近点を求める（AABB判定と同じロジック）
        Vector3 closestLocal = {
            std::clamp(localCenter.x, -halfSize.x, halfSize.x),
            std::clamp(localCenter.y, -halfSize.y, halfSize.y),
            std::clamp(localCenter.z, -halfSize.z, halfSize.z)
        };

        // 最近点をワールド座標に戻す
        Vector3 closestWorld = {
            obb.center.x + closestLocal.x * obb.orientation[0].x + closestLocal.y * obb.orientation[1].x + closestLocal.z * obb.orientation[2].x,
            obb.center.y + closestLocal.x * obb.orientation[0].y + closestLocal.y * obb.orientation[1].y + closestLocal.z * obb.orientation[2].y,
            obb.center.z + closestLocal.x * obb.orientation[0].z + closestLocal.y * obb.orientation[1].z + closestLocal.z * obb.orientation[2].z
        };

        // 球の中心と最近点の距離の2乗を計算
        Vector3 diff = {
            sphere.center.x - closestWorld.x,
            sphere.center.y - closestWorld.y,
            sphere.center.z - closestWorld.z
        };
        float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

        return distanceSq <= (sphere.radius * sphere.radius);
    }

    /// <summary>
    /// Sphere × OBB 判定（呼び出し順反転のラッパ）
    /// </summary>
    inline bool IsHit(const Sphere& sphere, const OBB& obb)
    {
        return IsHit(obb, sphere);
    }

    /// <summary>
    /// OvalSphere × Sphere 判定
    /// 球体の中心から楕円球体表面への最短距離で判定
    /// </summary>
    inline bool IsHit(const OvalSphere& ovalSphere, const Sphere& sphere)
    {
        // 楕円球体の中心から球体中心への差分ベクトル
        Vector3 diff = {
            sphere.center.x - ovalSphere.center.x,
            sphere.center.y - ovalSphere.center.y,
            sphere.center.z - ovalSphere.center.z
        };

        // 楕円座標系に変換（各軸を半径で正規化）
        Vector3 normalizedDiff = {
            diff.x / ovalSphere.radius.x,
            diff.y / ovalSphere.radius.y,
            diff.z / ovalSphere.radius.z
        };

        // 正規化された空間での距離
        float normalizedDistance = std::sqrt(normalizedDiff.x * normalizedDiff.x + 
                                           normalizedDiff.y * normalizedDiff.y + 
                                           normalizedDiff.z * normalizedDiff.z);

        // 楕円球体表面への最近点を正規化空間で計算
        if (normalizedDistance < 0.0001f) {
            // 球の中心が楕円の中心と同じ場合は必ず衝突
            return true;
        }

        Vector3 normalizedSurfacePoint = {
            normalizedDiff.x / normalizedDistance,
            normalizedDiff.y / normalizedDistance,
            normalizedDiff.z / normalizedDistance
        };

        // 楕円表面の点をワールド座標に戻す
        Vector3 surfacePoint = {
            ovalSphere.center.x + normalizedSurfacePoint.x * ovalSphere.radius.x,
            ovalSphere.center.y + normalizedSurfacePoint.y * ovalSphere.radius.y,
            ovalSphere.center.z + normalizedSurfacePoint.z * ovalSphere.radius.z
        };

        // 球の中心から楕円表面の最近点までの距離
        Vector3 surfaceDiff = {
            sphere.center.x - surfacePoint.x,
            sphere.center.y - surfacePoint.y,
            sphere.center.z - surfacePoint.z
        };

        float surfaceDistanceSq = surfaceDiff.x * surfaceDiff.x + 
                                 surfaceDiff.y * surfaceDiff.y + 
                                 surfaceDiff.z * surfaceDiff.z;

        return surfaceDistanceSq <= (sphere.radius * sphere.radius);
    }

    /// <summary>
    /// Sphere × OvalSphere 判定（呼び出し順反転のラッパ）
    /// </summary>
    inline bool IsHit(const Sphere& sphere, const OvalSphere& ovalSphere)
    {
        return IsHit(ovalSphere, sphere);
    }

    /// <summary>
    /// OvalSphere × AABB 判定
    /// 楕円球体の各軸での最近点を求めて判定
    /// </summary>
    inline bool IsHit(const OvalSphere& ovalSphere, const AABB& aabb)
    {
        // AABBのmin/max計算
        Vector3 aabbMin = {
            aabb.center.x - aabb.size.x * 0.5f,
            aabb.center.y - aabb.size.y * 0.5f,
            aabb.center.z - aabb.size.z * 0.5f
        };
        Vector3 aabbMax = {
            aabb.center.x + aabb.size.x * 0.5f,
            aabb.center.y + aabb.size.y * 0.5f,
            aabb.center.z + aabb.size.z * 0.5f
        };

        // AABBに対する楕円球体の最近点を求める
        Vector3 closestPoint = {
            std::clamp(ovalSphere.center.x, aabbMin.x, aabbMax.x),
            std::clamp(ovalSphere.center.y, aabbMin.y, aabbMax.y),
            std::clamp(ovalSphere.center.z, aabbMin.z, aabbMax.z)
        };

        // 楕円球体の中心から最近点への差分
        Vector3 diff = {
            closestPoint.x - ovalSphere.center.x,
            closestPoint.y - ovalSphere.center.y,
            closestPoint.z - ovalSphere.center.z
        };

        // 楕円の方程式で判定: (x/a)² + (y/b)² + (z/c)² <= 1
        float ellipseValue = (diff.x * diff.x) / (ovalSphere.radius.x * ovalSphere.radius.x) +
                           (diff.y * diff.y) / (ovalSphere.radius.y * ovalSphere.radius.y) +
                           (diff.z * diff.z) / (ovalSphere.radius.z * ovalSphere.radius.z);

        return ellipseValue <= 1.0f;
    }

    /// <summary>
    /// AABB × OvalSphere 判定（呼び出し順反転のラッパ）
    /// </summary>
    inline bool IsHit(const AABB& aabb, const OvalSphere& ovalSphere)
    {
        return IsHit(ovalSphere, aabb);
    }

    /// <summary>
    /// OvalSphere × OBB 判定
    /// 楕円球体をOBBのローカル座標系に変換して判定
    /// </summary>
    inline bool IsHit(const OvalSphere& ovalSphere, const OBB& obb)
    {
        // 楕円球体の中心をOBBのローカル座標系に変換
        Vector3 centerDiff = {
            ovalSphere.center.x - obb.center.x,
            ovalSphere.center.y - obb.center.y,
            ovalSphere.center.z - obb.center.z
        };

        // OBBの各軸に投影してローカル座標を求める
        Vector3 localCenter = {
            centerDiff * obb.orientation[0],
            centerDiff * obb.orientation[1],
            centerDiff * obb.orientation[2]
        };

        // OBBのローカル空間での半サイズ
        Vector3 halfSize = {
            obb.size.x * 0.5f,
            obb.size.y * 0.5f,
            obb.size.z * 0.5f
        };

        // ローカル空間での最近点を求める
        Vector3 closestLocal = {
            std::clamp(localCenter.x, -halfSize.x, halfSize.x),
            std::clamp(localCenter.y, -halfSize.y, halfSize.y),
            std::clamp(localCenter.z, -halfSize.z, halfSize.z)
        };

        // 最近点をワールド座標に戻す
        Vector3 closestWorld = {
            obb.center.x + closestLocal.x * obb.orientation[0].x + closestLocal.y * obb.orientation[1].x + closestLocal.z * obb.orientation[2].x,
            obb.center.y + closestLocal.x * obb.orientation[0].y + closestLocal.y * obb.orientation[1].y + closestLocal.z * obb.orientation[2].y,
            obb.center.z + closestLocal.x * obb.orientation[0].z + closestLocal.y * obb.orientation[1].z + closestLocal.z * obb.orientation[2].z
        };

        // 楕円球体の中心から最近点への差分
        Vector3 diff = {
            closestWorld.x - ovalSphere.center.x,
            closestWorld.y - ovalSphere.center.y,
            closestWorld.z - ovalSphere.center.z
        };

        // 楕円の方程式で判定: (x/a)² + (y/b)² + (z/c)² <= 1
        float ellipseValue = (diff.x * diff.x) / (ovalSphere.radius.x * ovalSphere.radius.x) +
                           (diff.y * diff.y) / (ovalSphere.radius.y * ovalSphere.radius.y) +
                           (diff.z * diff.z) / (ovalSphere.radius.z * ovalSphere.radius.z);

        return ellipseValue <= 1.0f;
    }

    /// <summary>
    /// OBB × OvalSphere 判定（呼び出し順反転のラッパ）
    /// </summary>
    inline bool IsHit(const OBB& obb, const OvalSphere& ovalSphere)
    {
        return IsHit(ovalSphere, obb);
    }

    /// <summary>
    /// OvalSphere × OvalSphere 判定
    /// 2つの楕円球体同士の衝突判定（近似解法）
    /// </summary>
    inline bool IsHit(const OvalSphere& a, const OvalSphere& b)
    {
        // 中心間の距離ベクトル
        Vector3 diff = {
            b.center.x - a.center.x,
            b.center.y - a.center.y,
            b.center.z - a.center.z
        };

        // 各軸での正規化された距離を計算
        // 保守的な判定のため、各楕円体の半径の和を使用
        float normalizedDistanceSq = 
            (diff.x * diff.x) / ((a.radius.x + b.radius.x) * (a.radius.x + b.radius.x)) +
            (diff.y * diff.y) / ((a.radius.y + b.radius.y) * (a.radius.y + b.radius.y)) +
            (diff.z * diff.z) / ((a.radius.z + b.radius.z) * (a.radius.z + b.radius.z));

        return normalizedDistanceSq <= 1.0f;
    }
}
