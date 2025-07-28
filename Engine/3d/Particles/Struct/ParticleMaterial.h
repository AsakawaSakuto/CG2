#pragma once
#include"Vector4.h"
#include"Matrix4x4.h"

struct ParticleMaterial {
    Vector4 color;            // 16B
    Matrix4x4 uvTransform;    // 64B
};