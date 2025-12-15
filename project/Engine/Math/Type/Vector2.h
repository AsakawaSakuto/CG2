#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 {
	float x;
	float y;

	Vector2& operator=(const Vector2& other) {
		if (this != &other) {
			x = other.x;
			y = other.y;
		}
		return *this;
	}
	Vector2& operator+=(const Vector2& num) {
		this->x += num.x;
		this->y += num.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& num) {
		this->x -= num.x;
		this->y -= num.y;
		return *this;
	}

	// 要素ごとの積を行う（*=）
	Vector2& operator*=(const Vector2& num) {
		this->x *= num.x;
		this->y *= num.y;
		return *this;
	}

	// ベクトル同士の加算（+）
	Vector2 operator+(const Vector2& other) const {
		Vector2 result = *this;
		result += other;
		return result;
	}

	// ベクトル同士の減算（-）
	Vector2 operator-(const Vector2& other) const {
		Vector2 result = *this;
		result -= other;
		return result;
	}

	// 各成分にスカラー値を加算する
	Vector2 operator+(float scalar) const {
		return Vector2{ x + scalar, y + scalar };
	}

	// 各成分からスカラー値を減算する
	Vector2 operator-(float scalar) const {
		return Vector2{ x - scalar, y - scalar };
	}

	// 各成分をスカラー倍する
	Vector2 operator*(float scalar) const {
		return Vector2{ x * scalar, y * scalar };
	}
};