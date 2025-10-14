#pragma once
#include"Vector3.h"
#include"Vector2.h"

// Emitter shape types (must match HLSL definitions)
enum class EmitterShapeType : uint32_t {
	POINT = 0,
	LINE = 1,
	SPHERE_VOLUME = 2,
	SPHERE_SURFACE = 3,
	BOX = 4,
	RING = 5,
	BOX_SURFACE = 6,
	RING_XY = 7,
	RING_YZ = 8,
	CONE = 9,
	CONE_SURFACE = 10,
	HEMISPHERE = 11,
	HEMISPHERE_SURFACE = 12
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
	
	// New fields for cone and hemisphere emitters
	float coneAngle;        // Cone angle in degrees (0-180)
	float coneHeight;       // Cone height
	Vector3 coneDirection;  // Cone direction vector
	float hemisphereAngle;  // Hemisphere angle in degrees (0-180)
};