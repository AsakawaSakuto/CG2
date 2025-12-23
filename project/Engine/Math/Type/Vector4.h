#pragma once
#include <cstddef>  // std::size_t
#include <cmath>    // std::sqrt

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    // --- constructors ---
    constexpr Vector4() = default;
    constexpr Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

    // --- index access ---
    constexpr float& operator[](std::size_t i) noexcept {
        return (i == 0) ? x : (i == 1 ? y : (i == 2 ? z : w));
    }
    constexpr const float& operator[](std::size_t i) const noexcept {
        return (i == 0) ? x : (i == 1 ? y : (i == 2 ? z : w));
    }

    // --- unary ---
    constexpr Vector4 operator+() const noexcept { return *this; }
    constexpr Vector4 operator-() const noexcept { return { -x, -y, -z, -w }; }

    // --- vector op ---
    constexpr Vector4 operator+(const Vector4& r) const noexcept { return { x + r.x, y + r.y, z + r.z, w + r.w }; }
    constexpr Vector4 operator-(const Vector4& r) const noexcept { return { x - r.x, y - r.y, z - r.z, w - r.w }; }
    constexpr Vector4 operator*(const Vector4& r) const noexcept { return { x * r.x, y * r.y, z * r.z, w * r.w }; } // 要素ごと
    constexpr Vector4 operator/(const Vector4& r) const noexcept { return { x / r.x, y / r.y, z / r.z, w / r.w }; } // 要素ごと

    constexpr Vector4& operator+=(const Vector4& r) noexcept { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
    constexpr Vector4& operator-=(const Vector4& r) noexcept { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }
    constexpr Vector4& operator*=(const Vector4& r) noexcept { x *= r.x; y *= r.y; z *= r.z; w *= r.w; return *this; }
    constexpr Vector4& operator/=(const Vector4& r) noexcept { x /= r.x; y /= r.y; z /= r.z; w /= r.w; return *this; }

    // --- scalar op ---
    constexpr Vector4 operator*(float s) const noexcept { return { x * s, y * s, z * s, w * s }; }
    constexpr Vector4 operator/(float s) const noexcept { return { x / s, y / s, z / s, w / s }; }
    constexpr Vector4& operator*=(float s) noexcept { x *= s; y *= s; z *= s; w *= s; return *this; }
    constexpr Vector4& operator/=(float s) noexcept { x /= s; y /= s; z /= s; w /= s; return *this; }

    friend constexpr Vector4 operator*(float s, const Vector4& v) noexcept { return { v.x * s, v.y * s, v.z * s, v.w * s }; }

    // --- compare ---
    constexpr bool operator==(const Vector4& r) const noexcept { return x == r.x && y == r.y && z == r.z && w == r.w; }
    constexpr bool operator!=(const Vector4& r) const noexcept { return !(*this == r); }

    // --- utilities ---
    constexpr float LengthSq() const noexcept { return x * x + y * y + z * z + w * w; }
    float Length() const noexcept { return std::sqrt(LengthSq()); }

    Vector4 Normalized() const noexcept {
        const float len = Length();
        if (len == 0.0f) { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
        return { x / len, y / len, z / len, w / len };
    }
    void Normalize() noexcept {
        const float len = Length();
        if (len == 0.0f) { return; }
        x /= len; y /= len; z /= len; w /= len;
    }

    static constexpr float Dot(const Vector4& a, const Vector4& b) noexcept {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
};