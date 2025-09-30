#pragma once
#include"Vector3.h"
#include"Vector4.h"

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float pad1;
	float intensity;
	uint32_t useLight;
	uint32_t useHalfLambert;
	float pad2;
};