#pragma once
#include"Vector3.h"
#include"Vector2.h"

// Emitter shape types (must match HLSL definitions)
enum class EmitterShapeType : uint32_t {
	POINT = 0,
	LINE = 1,
	SPHERE_VOLUME = 2,
	SPHERE_SURFACE = 3,
	BOX_VOLUME = 4,
	BOX_SURFACE = 5,
	RING_XZ = 6,
	RING_XY = 7,
	RING_YZ = 8
};

struct EmitterState {
	Vector3 translate;
	float radius;
	
	uint32_t useEmitter;
	uint32_t emit;
	uint32_t count;
	uint32_t kMaxParticle;

	float frequency;
	float frequencyTime;
	float pad2[2];

	Vector2 startScale;
	Vector2 endScale;

	uint32_t scaleFade;
	uint32_t scaleRandom;
	float pad3[2];

	Vector3 minScale;
	float pad4;
	Vector3 maxScale;
	float pad5;

	uint32_t rotateMove;
	float startRotateVelocity;
	float endRotateVelocity;
	uint32_t rotateVelocityRandom;

	float minRotateVelocity;
	float maxRotateVelocity;
	float pad6[2];

	uint32_t alphaFade;
	uint32_t colorFade;
	float pad7[2];

	Vector3 startColor;
	float pad8;
	Vector3 endColor;
	float pad9;

	uint32_t colorRandom;
	float pad10[3];

	Vector3 minColor;
	float pad11;
	Vector3 maxColor;
	float pad12;

	uint32_t isMove;
	float pad13[3];
	
	Vector3 startVelocity;
	float pad14;

	Vector3 endVelocity;
	float pad15;

	uint32_t velocityRandom;
	float pad16[3];

	Vector3 minVelocity;
	float pad17;
	Vector3 maxVelocity;
	float pad18;

	float lifeTime;
	uint32_t lifeTimeRandom;
	float minLifeTime;
	float maxLifeTime;
	
	uint32_t shapeType;
	Vector3 size;
	
	Vector3 lineStart;
	float lineLength;
	
	float ringInnerRadius;
	float ringOuterRadius;
	float padNew[2];
};