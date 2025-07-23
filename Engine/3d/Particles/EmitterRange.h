#pragma once
#include"Vector2.h"
#include"Vector3.h"

struct EmitterRange {
    Vector3 minScale;
    float pad1;
    Vector3 maxScale;
    float pad2;

    Vector3 minColor;
    float pad3;
    Vector3 maxColor;
    float pad4;

    Vector3 minVelocity;
    float pa5;
    Vector3 maxVelocity;
    float pad6;

    float minLifeTime;
    float maxLifeTime;
    Vector2 pad7;
};