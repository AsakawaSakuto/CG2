#pragma once
#include"Matrix4x4.h"

struct ModelTransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};