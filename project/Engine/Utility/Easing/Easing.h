#pragma once

#include <cmath>
#include <algorithm>
#include <numbers>

#include "Math/Type/Vector2.h"
#include "Math/Type/Vector3.h"

#include "type/EaseType.h"

/// <summary>
/// 汎用イージング関数群 float, Vector2, Vector3 対応
/// </summary>
namespace Easing {

    /// @brief イージング関数を適用
    /// @param t 進行状況 0.0～1.0
    /// @param type イージングタイプ
    /// @return イージング適用済み値 0.0～1.0
    float Apply(float t, EaseType type);

    /// @brief 任意型の補間 Start -> End
    /// @param start      開始ベクトル/値
    /// @param end       終了ベクトル/値
    /// @param t            進行状況 0.0～1.0
    /// @param goType イージングタイプ
    template<typename T>
    T Lerp(const T& start, const T& end, float t, EaseType type = EaseType::Linear) {
        float easedT = Apply(t, type);
        return start + (end - start) * easedT;
    }

    /// @brief 任意型の補間、行って帰ってくる
    /// @param start         開始ベクトル/値
    /// @param end          中間ベクトル/値
    /// @param t               進行状況 0.0～1.0
    /// @param goType    行きのイージング
    /// @param backType 帰りのイージング
    template<typename T>
    T Lerp_GAB(const T& start, const T& end, float t,
        EaseType goType = EaseType::Linear, EaseType backType = EaseType::Linear) {

        // 0〜1 に Clamp
        t = std::clamp(t, 0.0f, 1.0f);

        if (t < 0.5f) {
            // 行き：start → end
            float normalizedT = t * 2.0f;           // 0〜0.5 → 0〜1
            return Lerp<T>(start, end, normalizedT, goType);
        } else {
            // 帰り：end → start
            float normalizedT = (t - 0.5f) * 2.0f;  // 0.5〜1 → 0〜1
            return Lerp<T>(end, start, normalizedT, backType);
        }
    }
}
