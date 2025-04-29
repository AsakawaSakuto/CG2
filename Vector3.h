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

    float Length() const {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3 Normalize() const {
        float len = Length();
        return (len > 0) ? Vector3{ x / len, y / len, z / len } : Vector3{ 0, 0, 0 };
    }
};