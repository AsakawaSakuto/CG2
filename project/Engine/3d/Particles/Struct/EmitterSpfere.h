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
	uint32_t enableAlphaFade; // 透明度フェードを有効にするフラグ
	uint32_t enableScaleFade; // スケールフェードを有効にするフラグ
	float startScale; // 開始時のスケール倍率
	float endScale;   // 終了時のスケール倍率
};