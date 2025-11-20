#include "DescriptorAllocator.h"
#include "DirectXCommon.h" 

void DescriptorAllocator::Initialize(DirectXCommon* dx, uint32_t begin, uint32_t end) {
    dx_ = dx;
    begin_ = begin;
    end_ = end;
    next_ = begin_; // 先頭から順に使っても良いし、末尾から使いたければ next_=end_ にし、--する
    // 今回は"パーティクル専用レンジ内で先頭から詰めて確保"にします（衝突はレンジ分割で既に防止できている）
}

uint32_t DescriptorAllocator::Allocate() {
    // 返却済みがあれば再利用（穴埋め）
    if (!free_.empty()) {
        uint32_t idx = free_.top();
        free_.pop();
        return idx;
    }
    // 未消費スロットから確保
    assert(next_ <= end_ && "Particle SRV/UAV range exhausted");
    return next_++;
}

void DescriptorAllocator::Free(uint32_t index) {
    // レンジ内だけ返却許可
    assert(index >= begin_ && index <= end_);
    free_.push(index);
}