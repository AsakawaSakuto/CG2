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
};