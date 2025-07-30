#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"

struct Material {
    Vector4 color;
    int32_t enableLighting;
    Vector3 padding1;
    Matrix4x4 uvTransform;
    float shininess;
    Vector3 padding2;
};