#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include"WinApp.h"
#include"Logger.h"
#include"ConvertString.h"

class DirectXCommon
{
public:

	void Initialize(WinApp* winApp);

    // mainCpp用のゲッター達(後で消す)
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_; }
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() { return commandQueue_; }
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocator() { return commandAllocator_; }
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_; }
    Microsoft::WRL::ComPtr<IDXGISwapChain4> GetSwapChain() { return swapChain_; }
    DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; }
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsv() { return dsvDescriptorHeap_; }
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrv() { return srvDescriptorHeap_; }
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRtv() { return rtvDescriptorHeap_; }
    D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle0() { return rtvHandles_[0]; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle1() { return rtvHandles_[1]; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCPUHandle(uint32_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGPUHandle(uint32_t index);
    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCPUHandle(uint32_t index);
private:
    // 
    WinApp* winApp_ = nullptr;
    //
    HRESULT hr_;
    // デバイス生成
    void CreateDevice();
    Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_; // デバッグコントローラー
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;    // DXGIファクトリー
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;     // アダプタ
    Microsoft::WRL::ComPtr<ID3D12Device> device_;          // D3D12デバイス

    // コマンド生成
    void CreateCommand();
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;         // コマンドキュー
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_; // コマンドアロケーター
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;   // コマンドリスト
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;                       // フェンス
    uint64_t fenceValue_ = 0;
    HANDLE fenceEvent_ = nullptr;
    
    // スワップチェイン生成
    void CreateSwapChain();
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_ = {};

    // 深度バッファの生成
    void CreateDepthBuffer();
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

    // 各種デスクリプターヒープの生成
    void CreateDescriptorHeaps();

    // DescriptorsSizeを取得しておく
    uint32_t descriptorSizeSRV_;
    uint32_t descriptorSizeRTV_;
    uint32_t descriptorSizeDSV_;
    
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_; // RTV用のヒープでディスクリプタ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_; // SR用Vのヒープでディスクリプタ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_; // DSV用のヒープでディスクリプタ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

    // レンダーターゲットビューの生成
    void CreateRenderTargetView();
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]; // RTVを2つ作るのでディスクリプタを2つ用意
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
};