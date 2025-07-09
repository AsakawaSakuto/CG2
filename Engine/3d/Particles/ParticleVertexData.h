#pragma once
#include"Vector2.h"
#include"Vector4.h"

struct ParticleVertexData {
    Vector4 position;  // 16 bytes
    Vector2 texcoord;  // 8 bytes
    float padding0[2]; // 8 bytes → texcoordを16バイトに揃えるため
};