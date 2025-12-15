#pragma once
#include <cstdint>
#include <stack>
#include <d3d12.h>

class DirectXCommon;

class DescriptorAllocator {
public:

    // [begin..end] の閉区間レンジをこのアロケータが占有
    void Initialize(DirectXCommon* dx, uint32_t begin, uint32_t end);

    uint32_t Allocate();

    void Free(uint32_t index);

    // 使用中のSRV数を取得
    uint32_t GetUsedCount() const { return (next_ - begin_) - static_cast<uint32_t>(free_.size()); }
    
    // 総容量を取得
    uint32_t GetCapacity() const { return end_ - begin_ + 1; }

private:
    DirectXCommon* dx_ = nullptr;
    uint32_t begin_ = 0, end_ = 0;
    uint32_t next_ = 0;
    std::stack<uint32_t> free_;
};