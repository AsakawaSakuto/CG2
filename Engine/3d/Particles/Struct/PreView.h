#pragma once
#include"Matrix4x4.h"

struct PerView {
	Matrix4x4 viewProjection;
	Matrix4x4 billboardMatrix;
};