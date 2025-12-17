#pragma once
#include "Math/Type/Vector3.h"
#include "Math/Type/Vector2.h"
#include <string>
#include "Utility/BlendMode/BlendMode.h"

/// <summary>
/// Emitterの形状タイプ
/// </summary>
enum class EmitterShapeType : uint32_t {
	POINT = 0,   // 点
	LINE = 1,    // 線
	SPHERE = 2,  // 球
	BOX = 3,     // 箱
	PLANE = 4,   // 平面
	RING = 5,    // リング
};

/// <summary>
/// GPU互換のEmitter構造体（PODデータのみ）
/// </summary>
struct EmitterStateGPU {
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

	Vector3 normalVelocity;
	float pad19;

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
	float pad21[2];

	Vector3 planeNormal;
	float pad22;

	uint32_t spawnOnEdge;      // エッジ上に生成するか (0: 内部/表面全体, 1: エッジのみ)
	uint32_t enableVisualization; // 可視化を有効にするか (0: 無効, 1: 有効)
	uint32_t useGravity;
	uint32_t blendModeValue; // BlendModeをuint32_tとして格納

	float gravityY;
	float accelerationY;
	float pad23[2];
};

/// <summary>
/// Emitterの値を保持する構造体（CPU側で使用）
/// </summary>
struct EmitterState : public EmitterStateGPU {
	std::string texturePath;  // CPU専用
	BlendMode blendMode = kBlendModeAdd; // CPU専用
};