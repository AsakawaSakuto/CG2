#pragma once
#include"Math/Type/Vector3.h"

struct CameraForGPU {
    Vector3 worldPosition;
    float padding; // 16バイト整列
};
