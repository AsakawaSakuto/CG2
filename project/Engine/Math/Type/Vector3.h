#pragma once
#include <cstddef>  // std::size_t
#include <cmath>    // std::sqrt

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    // --- constructors ---
    constexpr Vector3() = default;
    constexpr Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    // --- index access ---
    constexpr float& operator[](std::size_t i) noexcept {
        return (i == 0) ? x : (i == 1 ? y : z);
    }
    constexpr const float& operator[](std::size_t i) const noexcept {
        return (i == 0) ? x : (i == 1 ? y : z);
    }

    // --- unary ---
    constexpr Vector3 operator+() const noexcept { return *this; }
    constexpr Vector3 operator-() const noexcept { return Vector3{ -x, -y, -z }; }

    // --- vector op ---
    constexpr Vector3 operator+(const Vector3& r) const noexcept { return { x + r.x, y + r.y, z + r.z }; }
    constexpr Vector3 operator-(const Vector3& r) const noexcept { return { x - r.x, y - r.y, z - r.z }; }
    constexpr Vector3 operator*(const Vector3& r) const noexcept { return { x * r.x, y * r.y, z * r.z }; } // 要素ごと
    constexpr Vector3 operator/(const Vector3& r) const noexcept { return { x / r.x, y / r.y, z / r.z }; } // 要素ごと

    constexpr Vector3& operator+=(const Vector3& r) noexcept { x += r.x; y += r.y; z += r.z; return *this; }
    constexpr Vector3& operator-=(const Vector3& r) noexcept { x -= r.x; y -= r.y; z -= r.z; return *this; }
    constexpr Vector3& operator*=(const Vector3& r) noexcept { x *= r.x; y *= r.y; z *= r.z; return *this; }
    constexpr Vector3& operator/=(const Vector3& r) noexcept { x /= r.x; y /= r.y; z /= r.z; return *this; }

    // --- scalar op ---
    constexpr Vector3 operator*(float s) const noexcept { return { x * s, y * s, z * s }; }
    constexpr Vector3 operator/(float s) const noexcept { return { x / s, y / s, z / s }; }
    constexpr Vector3& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
    constexpr Vector3& operator/=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }

    friend constexpr Vector3 operator*(float s, const Vector3& v) noexcept { return { v.x * s, v.y * s, v.z * s }; }

    // --- compare ---
    constexpr bool operator==(const Vector3& r) const noexcept { return x == r.x && y == r.y && z == r.z; }
    constexpr bool operator!=(const Vector3& r) const noexcept { return !(*this == r); }

    // --- utilities ---
    constexpr float LengthSq() const noexcept { return x * x + y * y + z * z; }
    float Length() const noexcept { return std::sqrt(LengthSq()); }

    Vector3 Normalized() const noexcept {
        const float len = Length();
        if (len == 0.0f) { return { 0.0f, 0.0f, 0.0f }; }
        return { x / len, y / len, z / len };
    }
    void Normalize() noexcept {
        const float len = Length();
        if (len == 0.0f) { return; }
        x /= len; y /= len; z /= len;
    }

    static constexpr float Dot(const Vector3& a, const Vector3& b) noexcept {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static constexpr Vector3 Cross(const Vector3& a, const Vector3& b) noexcept {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
};