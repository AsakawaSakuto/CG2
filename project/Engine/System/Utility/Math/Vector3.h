#pragma once
#include <cmath>
#include <limits>
#include <iostream>

/// <summary>
/// 3次元ベクトルを表す構造体
/// </summary>
struct Vector3 {
    float x;
    float y;
    float z;

    ///@return ベクトルの長さ（ノルム）を返す
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    ///@return 正規化ベクトルを返す（長さを1にしたベクトル）
    ///@return ゼロベクトルの場合は {0,0,0} を返す
    Vector3 Normalize() const {
        float len = Length();
        if (len == 0.0f) return { 0.0f, 0.0f, 0.0f }; // 0除算防止
        return { x / len, y / len, z / len };
    }

    /// 内積（dot product）
    ///@return 自分と other の角度・投影関係を表すスカラー値を返す
    float operator*(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    /// 外積（cross product）
    ///@return 自分と other に垂直なベクトルを返す
    Vector3 operator^(const Vector3& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    // 代入演算子
    Vector3& operator=(const Vector3& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    // 自身に他のベクトルを加算する（+=）
    Vector3& operator+=(const Vector3& num) {
        this->x += num.x;
        this->y += num.y;
        this->z += num.z;
        return *this;
    }

    // 自身から他のベクトルを減算する（-=）
    Vector3& operator-=(const Vector3& num) {
        this->x -= num.x;
        this->y -= num.y;
        this->z -= num.z;
        return *this;
    }

    // 要素ごとの積を行う（*=）
    // ※通常の数学的ベクトル積ではない（Hadamard積）
    Vector3& operator*=(const Vector3& num) {
        this->x *= num.x;
        this->y *= num.y;
        this->z *= num.z;
        return *this;
    }

    // ベクトル同士の加算（+）
    Vector3 operator+(const Vector3& other) const {
        Vector3 result = *this;
        result += other;
        return result;
    }

    // ベクトル同士の減算（-）
    Vector3 operator-(const Vector3& other) const {
        Vector3 result = *this;
        result -= other;
        return result;
    }

    // 各成分にスカラー値を加算する
    Vector3 operator+(float scalar) const {
        return Vector3{ x + scalar, y + scalar, z + scalar };
    }

    // 各成分からスカラー値を減算する
    Vector3 operator-(float scalar) const {
        return Vector3{ x - scalar, y - scalar, z - scalar };
    }

    // 各成分をスカラー倍する
    Vector3 operator*(float scalar) const {
        return Vector3{ x * scalar, y * scalar, z * scalar };
    }
};

// スカラー * ベクトル
inline Vector3 operator*(float scalar, const Vector3& vec) {
    return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
}