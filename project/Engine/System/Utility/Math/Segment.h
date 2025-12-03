#pragma once
#include "Vector3.h"

// 線分
struct Segment {
	Vector3 origin = { 0.0f,0.0f,0.0f }; // 始点
	Vector3 diff = { 1.0f,1.0f,1.0f };   // 終点への差分ベクトル
};