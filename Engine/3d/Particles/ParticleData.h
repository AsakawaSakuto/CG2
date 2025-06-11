#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Transform.h"

struct ParticleData
{
	Transform transform; // トランスフォーム
	Vector3 velocity;    // 速度
	Vector4 color;       // 色
	float lifeTime;      // 生存可能な時間
	float currentTime;   // 発生してからの経過時間
};