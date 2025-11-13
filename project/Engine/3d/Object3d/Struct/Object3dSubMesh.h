#pragma once
#include "vector3.h"

struct ModelSubMesh {
    uint32_t indexStart;
    uint32_t indexCount;
    uint32_t materialIndex;
};