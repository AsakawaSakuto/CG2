#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"

struct ModelMaterial {
    Vector4 color;
    int32_t enableLighting;
    float pad1[3];
    Matrix4x4 uvTransformMatrix;
    float shininess;
    float pad2[3];
};