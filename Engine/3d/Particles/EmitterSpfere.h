#pragma once
#include"Vector3.h"

struct EmitterSphere
{
	Vector3 translate;
	float radius;
	int count;
	float frequency;
	float frequencyTime;
	uint32_t emit;
	float pad[2];
};