#pragma once
#include"Matrix4x4.h"

struct Object3dTransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};