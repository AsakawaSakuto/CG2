#pragma once

#include <cmath>
#include <algorithm>
#include <numbers>

#include"Vector2.h"
#include"Vector3.h"

/// イージング関数ユーティリティ
namespace Easing {

    /// イージングタイプ
    enum class Type {
        Linear, // 等速：開始も終了も速度が一定で直線的に変化

        // Quadratic (2乗) - 基本的な加速・減速

        EaseInQuad,    // 緩やかに始まり徐々に加速する
        EaseOutQuad,   // 速く始まり徐々に減速する
        EaseInOutQuad, // 始めと終わりが緩やかで中央で加速する（S字状）

        // Cubic (3乗) - より強い加速・減速

        EaseInCubic, // 非常に緩やかに始まり中盤で強く加速する
        EaseOutCubic, // 先に速く進み終盤で強く減速する
        EaseInOutCubic, // 両端が緩やかで中央がより強く変化する（はっきりしたS字）

        // Quartic (4乗) - さらに強い加速・減速

        EaseInQuart, // 極めて緩やかに始まり後半で急激に加速する
        EaseOutQuart, // 先に急速に進み終盤で極めて緩やかに減速する
        EaseInOutQuart, // 両端が非常に緩やかで中央で急激に変化する

        // Quintic (5乗) - 最も強い加速・減速 

        EaseInQuint, // ごくゆっくり始まり終盤で急激に加速する
        EaseOutQuint, // 先に非常に急速に進み終盤でごく緩やかに止まる
        EaseInOutQuint, // 非常に強いS字で両端が緩やか、中央で最大の変化

        // Sine - 滑らかな曲線（自然な動き）

        EaseInSine, // 正弦状で滑らかに始まり穏やかに加速する
        EaseOutSine, // 正弦状で滑らかに始まり終盤で穏やかに減速する
        EaseInOutSine, // 正弦状のS字で自然で滑らかな変化

        // Exponential - 急激な変化（デジタル的） 

        EaseInExpo, // 非常に急激に加速し始める（ほとんどゼロから急上昇）
        EaseOutExpo, // 終盤で急速に減速してほぼ瞬時に止まる
        EaseInOutExpo, // 両端が緩やかで中央が指数的に急変する

        // Circular - 円弧状の変化（幾何学的）

        EaseInCirc, // 円弧的に緩やかに始まり中盤で加速する（滑らかな曲線）
        EaseOutCirc, // 先に速く進み終盤は円弧的に緩やかに止まる
        EaseInOutCirc, // 両端が緩やかで中央が円弧的に強く変化する

        // Back - オーバーシュート（行き過ぎて戻る）

        EaseInBack, // 少し逆方向に引いてから目標へ向かい加速する（予備動作）
        EaseOutBack, // 目標を少し超えてから戻る（弾力のある着地）
        EaseInOutBack, // 両端で軽くオーバーシュートするダイナミックなS字

        // Elastic - 弾性（ゴムのような反発）

        EaseInElastic, // 振動を伴いながら加速していく（伸び縮みする挙動）
        EaseOutElastic, // 目標で数回振動して収束する（バネの反発）
        EaseInOutElastic, // 開始と終了で振動を伴う強い弾性挙動

        // Bounce - 跳ね返り（ボールの弾み）

        EaseInBounce, // 跳ねながら加速していく（複数回のバウンド）
        EaseOutBounce, // 地面で何度か弾んで最終的に止まる
        EaseInOutBounce // 両端でバウンドする遊びのある動き

    };

    /// @brief 2つの値の間を補間
    /// @param start 開始値
    /// @param end 終了値
    /// @param t 進行状況（0.0～1.0）
    /// @param type イージングタイプ
    /// @return 補間された値
    float Lerp(float start, float end, float t, Type type = Type::Linear);

    /// @brief Vector2の補間
    /// @param start 開始ベクトル
    /// @param end 終了ベクトル
    /// @param t 進行状況（0.0～1.0）
    /// @param type イージングタイプ
    /// @return 補間されたベクトル
    Vector2 LerpVector2(const Vector2& start, const Vector2& end, float t, Type type = Type::Linear);

    /// @brief Vector3の補間
    /// @param start 開始ベクトル
    /// @param end 終了ベクトル
    /// @param t 進行状況（0.0～1.0）
    /// @param type イージングタイプ
    /// @return 補間されたベクトル
    Vector3 LerpVector3(const Vector3& start, const Vector3& end, float t, Type type = Type::Linear);

    /// @brief イージング関数を適用
    /// @param t 進行状況（0.0～1.0）
    /// @param type イージングタイプ
    /// @return イージング適用済み値（0.0～1.0）
    float Apply(float t, Type type);
}