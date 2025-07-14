#pragma once
#include"Vector3.h"
#include"Vector4.h"

struct ParticleDataCS {
    Vector3 scale;       // 12 bytes
    float pad1;          //  4 bytes → 合計16バイト

    Vector3 rotate;      // 12 bytes
    float pad2;          //  4 bytes

    Vector3 translate;   // 12 bytes
    float pad3;          //  4 bytes

    Vector3 velocity;    // 12 bytes
    float pad4;          //  4 bytes

    float lifeTime;      //  4 bytes
    float currentTime;   //  4 bytes
    float pad5[2];       //  8 bytes → 合計16バイト

    Vector4 color;       // 16 bytes
};