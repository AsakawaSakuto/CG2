#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"

struct VertexData {
	Vector4 position;  // 16 bytes
	Vector2 texcoord;  // 8 bytes
	Vector3 normal;    // 12 bytes
	float _padding[2]; // 8 bytes (align to 16)
	float _pad2;       // 4 bytes (align to 16 again)
};