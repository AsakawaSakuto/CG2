#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <queue>
#include <cassert>
#include <memory>

class HeapManager {
public:
    // シングルトン
    static HeapManager* GetInstance();

    //
    void Initialize(ID3D12Device* device, uint32_t maxSRV, uint32_t maxUAV);

    // SRVヒープ
    ID3D12DescriptorHeap* GetSRVHeap() const { return srvHeap_.Get(); }
    
    // UAVヒープ
    ID3D12DescriptorHeap* GetUAVHeap() const { return uavHeap_.Get(); }

    // SRV割り当て
    uint32_t AllocateSRVSlot();
    void FreeSRVSlot(uint32_t index);

    // UAV割り当て
    uint32_t AllocateUAVSlot();
    void FreeUAVSlot(uint32_t index);

    // ハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCPUHandle(uint32_t index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGPUHandle(uint32_t index) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetUavCPUHandle(uint32_t index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetUavGPUHandle(uint32_t index) const;

    uint32_t GetSRVDescriptorSize() const { return descriptorSizeSRV_; }
    uint32_t GetUAVDescriptorSize() const { return descriptorSizeUAV_; }

private:
    HeapManager() = default;
    ~HeapManager() = default;
    HeapManager(const HeapManager&) = delete;
    HeapManager& operator=(const HeapManager&) = delete;

    struct Deleter {
        void operator()(HeapManager* ptr) const {
            delete ptr;
        }
    };

    static std::unique_ptr<HeapManager, Deleter> instance;

    // SRV/UAVヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> uavHeap_;

    // スロット管理
    std::queue<uint32_t> freeSRVSlots_;
    std::queue<uint32_t> freeUAVSlots_;
    uint32_t maxSRV_ = 0;
    uint32_t maxUAV_ = 0;

    // SRV/UAVディスクリプタサイズ
    uint32_t descriptorSizeSRV_ = 0;
    uint32_t descriptorSizeUAV_ = 0;
};
