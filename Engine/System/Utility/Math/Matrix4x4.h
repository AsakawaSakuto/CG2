#pragma once

/// <summary>
/// 4x4配列
/// </summary>
struct Matrix4x4 {
	float m[4][4];

    Matrix4x4& operator*(const Matrix4x4& rhs) {
        Matrix4x4 result = {};
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                result.m[row][col] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.m[row][col] += m[row][k] * rhs.m[k][col];
                }
            }
        }
        *this = result;
        return *this;
    }
};