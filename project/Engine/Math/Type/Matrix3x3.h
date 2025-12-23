#pragma once

/// <summary>
/// 3x3配列
/// </summary>
struct Matrix3x3 {
    float m[3][3];

    // 加算
    Matrix3x3 operator+(const Matrix3x3& rhs) const {
        Matrix3x3 result{};
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                result.m[y][x] = m[y][x] + rhs.m[y][x];
            }
        }
        return result;
    }

    // 減算
    Matrix3x3 operator-(const Matrix3x3& rhs) const {
        Matrix3x3 result{};
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                result.m[y][x] = m[y][x] - rhs.m[y][x];
            }
        }
        return result;
    }

    // 行列積
    Matrix3x3 operator*(const Matrix3x3& rhs) const {
        Matrix3x3 result{};
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                result.m[row][col] = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    result.m[row][col] += m[row][k] * rhs.m[k][col];
                }
            }
        }
        return result;
    }
};