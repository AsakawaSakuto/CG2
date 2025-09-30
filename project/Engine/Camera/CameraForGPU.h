#pragma once
#include"Vector3.h"

struct CameraForGPU {
    Vector3 worldPosition;
    float padding; // 16バイト整列
};
