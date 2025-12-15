#pragma once
#include"Math/Type/Vector3.h"
#include"Math/Type/Vector4.h"

struct PointLight {
    Vector4 color;    // ライトの色
    Vector3 position; // ライトの位置
    float pad1;
    float intensity;  // 輝度
    float radius;     // 光の届く最大距離
    float decay;      // 減衰の指数
    uint32_t useLight;
};