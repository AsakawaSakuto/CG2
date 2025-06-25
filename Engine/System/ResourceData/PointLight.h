#pragma once
#include"Vector3.h"
#include"Vector4.h"

struct PointLight {
    Vector4 color;    // ライトの色
    Vector3 position; // ライトの位置
    float intensity;  // 輝度
    float  radius;    // 光の届く最大距離
    float  decay;     // 減衰の指数
};