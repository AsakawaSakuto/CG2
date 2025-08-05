#pragma once
#include "vector3.h"

struct Object3dSubMesh {
    uint32_t indexStart;
    uint32_t indexCount;
    uint32_t materialIndex;
};