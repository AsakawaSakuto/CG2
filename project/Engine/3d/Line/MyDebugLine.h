#pragma once
#include "LineManager.h"
#include "Utility/Collision/CollisionShape.h"
#include "Math/Type/Vector4.h"
#include "Camera/Camera.h"

namespace MyDebugLine {
    /// <summary>
    /// 形状を追加（統一API）
    /// </summary>
    /// <param name="shape">描画する形状（AABB, OBB, Sphere, OvalSphere, Plane, Segment, Line）</param>
    /// <param name="color">描画色（デフォルト：白）</param>
    inline void AddShape(const Shape& shape, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f }) {
        LineManager::GetInstance()->AddShape(shape, color);
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="size"></param>
    /// <param name="divisions"></param>
    /// <param name="color"></param>
    inline void AddGrid(float size, int divisions, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f }) {
        LineManager::GetInstance()->AddGrid(size, divisions, color);
	}

    /// <summary>
    /// すべての線を描画
    /// </summary>
    /// <param name="camera">カメラ</param>
    inline void Draw(Camera& camera) {
        LineManager::GetInstance()->Draw(camera);
    }
}
