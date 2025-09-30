#pragma once
#include <cstdint>
#include <stack>
#include <d3d12.h>

class DirectXCommon;

class ParticleDescriptorAllocator {
public:

    // [begin..end] の閉区間レンジをこのアロケータが占有
    void Initialize(DirectXCommon* dx, uint32_t begin, uint32_t end);

    uint32_t Allocate();

    void Free(uint32_t index);

private:
    DirectXCommon* dx_ = nullptr;
    uint32_t begin_ = 0, end_ = 0;
    uint32_t next_ = 0;
    std::stack<uint32_t> free_;
};