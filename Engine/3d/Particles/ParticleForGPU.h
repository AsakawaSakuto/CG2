#pragma once
#include"Vector4.h"
#include"Matrix4x4.h"

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};