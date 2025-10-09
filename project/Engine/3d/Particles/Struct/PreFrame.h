#pragma once

struct PerFrame {
    float time;
    float deltaTime;
    uint32_t index;
    float pad1;  // 16バイト境界に合わせるためのパディング
};