#pragma once
#include <cstddef>  // std::size_t
#include <cmath>    // std::sqrt

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;

    // --- constructors ---
    constexpr Vector2() = default;
    constexpr Vector2(float x_, float y_) : x(x_), y(y_) {}

    // --- index access ---
    constexpr float& operator[](std::size_t i) noexcept { return (i == 0) ? x : y; }
    constexpr const float& operator[](std::size_t i) const noexcept { return (i == 0) ? x : y; }

    // --- unary ---
    constexpr Vector2 operator+() const noexcept { return *this; }
    constexpr Vector2 operator-() const noexcept { return Vector2{ -x, -y }; }

    // --- vector op ---
    constexpr Vector2 operator+(const Vector2& r) const noexcept { return { x + r.x, y + r.y }; }
    constexpr Vector2 operator-(const Vector2& r) const noexcept { return { x - r.x, y - r.y }; }
    constexpr Vector2 operator*(const Vector2& r) const noexcept { return { x * r.x, y * r.y }; } // 要素ごと
    constexpr Vector2 operator/(const Vector2& r) const noexcept { return { x / r.x, y / r.y }; } // 要素ごと

    constexpr Vector2& operator+=(const Vector2& r) noexcept { x += r.x; y += r.y; return *this; }
    constexpr Vector2& operator-=(const Vector2& r) noexcept { x -= r.x; y -= r.y; return *this; }
    constexpr Vector2& operator*=(const Vector2& r) noexcept { x *= r.x; y *= r.y; return *this; }
    constexpr Vector2& operator/=(const Vector2& r) noexcept { x /= r.x; y /= r.y; return *this; }

    // --- scalar op ---
    constexpr Vector2 operator*(float s) const noexcept { return { x * s, y * s }; }
    constexpr Vector2 operator/(float s) const noexcept { return { x / s, y / s }; }
    constexpr Vector2& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
    constexpr Vector2& operator/=(float s) noexcept { x /= s; y /= s; return *this; }

    friend constexpr Vector2 operator*(float s, const Vector2& v) noexcept { return { v.x * s, v.y * s }; }

    // --- compare ---
    constexpr bool operator==(const Vector2& r) const noexcept { return x == r.x && y == r.y; }
    constexpr bool operator!=(const Vector2& r) const noexcept { return !(*this == r); }

    // --- utilities ---
    constexpr float LengthSq() const noexcept { return x * x + y * y; }
    float Length() const noexcept { return std::sqrt(LengthSq()); }

    /// <summary>正規化（ゼロ長なら {0,0} を返す）</summary>
    Vector2 Normalized() const noexcept {
        const float len = Length();
        if (len == 0.0f) { return { 0.0f, 0.0f }; }
        return { x / len, y / len };
    }
    /// <summary>正規化（ゼロ長なら何もしない）</summary>
    void Normalize() noexcept {
        const float len = Length();
        if (len == 0.0f) { return; }
        x /= len; y /= len;
    }

    static constexpr float Dot(const Vector2& a, const Vector2& b) noexcept { return a.x * b.x + a.y * b.y; }

    /// <summary>2Dの外積（スカラー）：a.x*b.y - a.y*b.x</summary>
    static constexpr float Cross(const Vector2& a, const Vector2& b) noexcept { return a.x * b.y - a.y * b.x; }
};
