#pragma once
#include"Math/Type/Vector3.h"
#include"Math/Type/Vector4.h"

struct SpotLight {
    Vector4 color;     // ライトの色
    Vector3 position;  // ライトの位置
    float intensity;   // 輝度
    Vector3 direction; // スポットライトの方向
    float distance;    // ライトの届く最大距離
    float decay;       // 減衰率
    float cosAngle;    // スポットライトの余弦
    float cosFalloffStart;
    uint32_t useLight;
};