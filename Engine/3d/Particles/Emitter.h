#pragma once
#include"Transform.h"

struct Emitter {
	Transform transform; // エミッターのトランスフォーム
	uint32_t count;      // 発生数
	float frequency;      // 発生頻度
	float frequencyTime;  // 頻度用時刻
};