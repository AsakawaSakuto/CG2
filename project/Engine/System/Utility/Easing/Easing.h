#pragma once

#include <cmath>
#include <algorithm>
#include <numbers>

#include"Vector2.h"
#include"Vector3.h"

// イージング実装の際に参考にしたサイト
// https://aketama.work/easing-list

/// 汎用イージング関数
namespace Easing {

    /// イージングのタイプ
    enum class Type {

        // 等速

        Linear,

        // Quadratic

        EaseInQuad,
        EaseOutQuad,
        EaseInOutQuad,
        EaseOutInQuad,

        // Cubic

        EaseInCubic,
        EaseOutCubic,
        EaseInOutCubic,
        EaseOutInCubic,

        // Quartic

        EaseInQuart,
        EaseOutQuart,
        EaseInOutQuart,
        EaseOutInQuart,

        // Quintic

        EaseInQuint,
        EaseOutQuint,
        EaseInOutQuint,
        EaseOutInQuint,

        // Sine

        EaseInSine,
        EaseOutSine,
        EaseInOutSine,
        EaseOutInSine,

        // Exponential

        EaseInExpo,
        EaseOutExpo,
        EaseInOutExpo,
        EaseOutInExpo,

        // Circular

        EaseInCirc,
        EaseOutCirc,
        EaseInOutCirc,
        EaseOutInCirc,

        // Back

        EaseInBack,
        EaseOutBack,
        EaseInOutBack,
        EaseOutInBack,

        // Elastic

        EaseInElastic,
        EaseOutElastic,
        EaseInOutElastic,
        EaseOutInElastic,

        // Bounce

        EaseInBounce,
        EaseOutBounce,
        EaseInOutBounce,
        EaseOutInBounce
    };

    /// @brief 2つの値の間を補間
    /// @param start 開始値
    /// @param end 終了値
    /// @param t 進行状況 0.0～1.0
    /// @param type イージングタイプ
    /// @return 補間された値
    float Lerp(float start, float end, float t, Type type = Type::Linear);

    /// @brief Vector2の補間
    /// @param start 開始ベクトル
    /// @param end 終了ベクトル
    /// @param t 進行状況 0.0～1.0
    /// @param type イージングタイプ
    /// @return 補間されたベクトル
    Vector2 LerpVector2(const Vector2& start, const Vector2& end, float t, Type type = Type::Linear);

    /// @brief Vector3の補間
	/// @param start 開始ベクトル
    /// @param end 終了ベクトル
    /// @param t 進行状況 0.0～1.0
    /// @param type イージングタイプ
    /// @return 補間されたベクトル
    Vector3 LerpVector3(const Vector3& start, const Vector3& end, float t, Type type = Type::Linear);

    /// @brief イージング関数を適用
    /// @param t 進行状況 0.0～1.0
    /// @param type イージングタイプ
    /// @return イージング適用済み値 0.0～1.0
    float Apply(float t, Type type);
}