#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"

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

    float rotateVelocity; // 4 bytes
    float pad6[3];        // 12 bytes → 合計16バイト

    Vector3 saveScale;
    float pad7;
};

struct ParticleMaterial {
    Vector4 color;            // 16B
    Matrix4x4 uvTransform;    // 64B
};

struct ParticleVertexData {
    Vector4 position;  // 16 bytes
    Vector2 texcoord;  // 8 bytes
    float padding0[2]; // 8 bytes → texcoordを16バイトに揃えるため
};

struct PerFrame {
    float time;
    float deltaTime;
    uint32_t index;
    float pad1;  // 16バイト境界に合わせるためのパディング
};

struct PerView {
    Matrix4x4 viewProjection;
    Matrix4x4 billboardMatrix;
};