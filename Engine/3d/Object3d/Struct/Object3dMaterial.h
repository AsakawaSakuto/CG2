#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"

struct Object3dMaterial {
    Vector4 color;
    int32_t enableLighting;
    float pad1[3];
    Matrix4x4 uvTransform;
    float shininess;
    float pad2[3];
};