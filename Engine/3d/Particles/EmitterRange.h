#pragma once
#include"Vector2.h"
#include"Vector3.h"

struct EmitterRange {
    Vector3 minScale;
    float pad1;
    Vector3 maxScale;
    float pad2;

    Vector3 minTranslate;
    float pad3;
    Vector3 maxTranslate;
    float pad4;

    Vector3 minColor;
    float pad5;
    Vector3 maxColor;
    float pad6;

    Vector3 minVelocity;
    float pad7;
    Vector3 maxVelocity;
    float pad8;

    float minLifeTime;
    float maxLifeTime;
    float pad9[2];
};