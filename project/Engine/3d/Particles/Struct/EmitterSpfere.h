#pragma once
#include"Vector3.h"

struct EmitterSphere {
	Vector3 translate;
	float radius;
	int count;
	float spawnTime;
	float spawnTimer;
	uint32_t emit;
	uint32_t kMaxParticle;
	uint32_t isMove;
};