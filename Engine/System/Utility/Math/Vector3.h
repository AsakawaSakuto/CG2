#pragma once
#include <cmath>
#include <limits>
#include <iostream>

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 {
	float x;
	float y;
	float z;

	// 長さを返す関数
	float Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	// 正規化されたVector3を返す
	Vector3 Normalize() const {
		float len = Length();
		if (len == 0.0f) return { 0.0f, 0.0f, 0.0f }; // 0除算防止
		return { x / len, y / len, z / len };
	}

	Vector3& operator=(const Vector3& other) {
		if (this != &other) {
			x = other.x;
			y = other.y;
			z = other.z;
		}
		return *this;
	}

	Vector3& operator+=(const Vector3& num) {
		this->x += num.x;
		this->y += num.y;
		this->z += num.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& num) {
		this->x -= num.x;
		this->y -= num.y;
		this->z -= num.z;
		return *this;
	}

	Vector3 operator+(float scalar) const {
		return Vector3{ x + scalar, y + scalar, z + scalar };
	}
};