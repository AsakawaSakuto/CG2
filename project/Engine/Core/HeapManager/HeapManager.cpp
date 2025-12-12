#include "HeapManager.h"

HeapManager* HeapManager::instance = nullptr;

HeapManager* HeapManager::GetInstance() {
    if (!instance) {
        instance = new HeapManager();
    }
    return instance;
}

void HeapManager::Initialize(ID3D12Device* device, uint32_t maxSRV, uint32_t maxUAV) {
    maxSRV_ = maxSRV;
    maxUAV_ = maxUAV;

    // SRVヒープ生成
    D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
    srvDesc.NumDescriptors = maxSRV;
    srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&srvHeap_));
    descriptorSizeSRV_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // UAVヒープ生成
    D3D12_DESCRIPTOR_HEAP_DESC uavDesc = {};
    uavDesc.NumDescriptors = maxUAV;
    uavDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    uavDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&uavDesc, IID_PPV_ARGS(&uavHeap_));
    descriptorSizeUAV_ = descriptorSizeSRV_; // 同じヒープタイプなのでSRVと同じ

    // スロット初期化
    for (uint32_t i = 0; i < maxSRV; ++i) freeSRVSlots_.push(i);
    for (uint32_t i = 0; i < maxUAV; ++i) freeUAVSlots_.push(i);
}

// ----- SRV -----
uint32_t HeapManager::AllocateSRVSlot() {
    assert(!freeSRVSlots_.empty());
    uint32_t idx = freeSRVSlots_.front();
    freeSRVSlots_.pop();
    return idx;
}
void HeapManager::FreeSRVSlot(uint32_t index) {
    assert(index < maxSRV_);
    freeSRVSlots_.push(index);
}

// ----- UAV -----
uint32_t HeapManager::AllocateUAVSlot() {
    assert(!freeUAVSlots_.empty());
    uint32_t idx = freeUAVSlots_.front();
    freeUAVSlots_.pop();
    return idx;
}
void HeapManager::FreeUAVSlot(uint32_t index) {
    assert(index < maxUAV_);
    freeUAVSlots_.push(index);
}

// ----- ハンドル取得 -----
D3D12_CPU_DESCRIPTOR_HANDLE HeapManager::GetSrvCPUHandle(uint32_t index) const {
    D3D12_CPU_DESCRIPTOR_HANDLE handle = srvHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSizeSRV_;
    return handle;
}
D3D12_GPU_DESCRIPTOR_HANDLE HeapManager::GetSrvGPUHandle(uint32_t index) const {
    D3D12_GPU_DESCRIPTOR_HANDLE handle = srvHeap_->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSizeSRV_;
    return handle;
}
D3D12_CPU_DESCRIPTOR_HANDLE HeapManager::GetUavCPUHandle(uint32_t index) const {
    D3D12_CPU_DESCRIPTOR_HANDLE handle = uavHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSizeUAV_;
    return handle;
}
D3D12_GPU_DESCRIPTOR_HANDLE HeapManager::GetUavGPUHandle(uint32_t index) const {
    D3D12_GPU_DESCRIPTOR_HANDLE handle = uavHeap_->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSizeUAV_;
    return handle;
}
