#pragma once
#include"Vector3.h"

struct EmitterSphere {
	Vector3 translate;        // 12 bytes
	float radius;             // 4 bytes -> 16バイト境界
	
	uint32_t count;           // 4 bytes
	float frequency;          // 4 bytes (HLSL側と合わせるために追加)
	float frequencyTime;      // 4 bytes (HLSL側と合わせるために追加)  
	uint32_t emit;            // 4 bytes -> 16バイト境界
	
	uint32_t kMaxParticle;    // 4 bytes
	uint32_t isMove;          // 4 bytes
	uint32_t enableAlphaFade; // 4 bytes
	uint32_t enableScaleFade; // 4 bytes -> 16バイト境界
	
	float startScale;         // 4 bytes
	float endScale;           // 4 bytes
	uint32_t enableColorFade; // 4 bytes
	float pad1;               // 4 bytes パディング -> 16バイト境界
	
	Vector3 startColor;       // 12 bytes
	float pad2;               // 4 bytes パディング -> 16バイト境界
	
	Vector3 endColor;         // 12 bytes
	float pad3;               // 4 bytes パディング -> 16バイト境界
};