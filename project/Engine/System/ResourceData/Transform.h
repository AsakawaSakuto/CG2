#pragma once
#include"../Quaternion/Quaternion.h"
#include"Vector3.h"
#include"Vector2.h"

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

struct UvTransform {
	Vector2 scale;
	float rotate;
	Vector2 translate;
};