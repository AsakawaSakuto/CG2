#pragma once

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4 {
	float x;
	float y;
	float z;
	float w;

    // 自身に他のベクトルを加算する（+=）
    Vector4& operator+=(const Vector4& num) {
        this->x += num.x;
        this->y += num.y;
        this->z += num.z;
        return *this;
    }

    // 自身から他のベクトルを減算する（-=）
    Vector4& operator-=(const Vector4& num) {
        this->x -= num.x;
        this->y -= num.y;
        this->z -= num.z;
        return *this;
    }

    // 要素ごとの積を行う（*=）
    // ※通常の数学的ベクトル積ではない（Hadamard積）
    Vector4& operator*=(const Vector4& num) {
        this->x *= num.x;
        this->y *= num.y;
        this->z *= num.z;
        return *this;
    }

    // ベクトル同士の加算（+）
    Vector4 operator+(const Vector4& other) const {
        Vector4 result = *this;
        result += other;
        return result;
    }

    // ベクトル同士の減算（-）
    Vector4 operator-(const Vector4& other) const {
        Vector4 result = *this;
        result -= other;
        return result;
    }

    // 各成分にスカラー値を加算する
    Vector4 operator+(float scalar) const {
        return Vector4{ x + scalar, y + scalar, z + scalar };
    }

    // 各成分からスカラー値を減算する
    Vector4 operator-(float scalar) const {
        return Vector4{ x - scalar, y - scalar, z - scalar };
    }

    // 各成分をスカラー倍する
    Vector4 operator*(float scalar) const {
        return Vector4{ x * scalar, y * scalar, z * scalar };
    }
};

// スカラー * ベクトル
inline Vector4 operator*(float scalar, const Vector4& vec) {
    return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
}