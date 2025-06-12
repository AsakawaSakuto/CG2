#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <wrl/client.h>
#include "externals/DirectXTex/DirectXTex.h"
#pragma comment(lib, "dxcompiler.lib")
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")
#include <dxcapi.h>                  
#pragma comment(lib,"dxcompiler.lib")
#include <chrono>
#include <thread>

#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"

#include"WinApp.h"
#include"Logger.h"
#include"ConvertString.h"
#include"CreateResource.h"

class DirectXCommon
{
public:

    // DirectXの初期化
	void Initialize(WinApp* winApp);

    // DirectXの描画前処理
    void PreDraw();

    //
    void PostDraw();

    //
    void ResetCommand();

    //
    void CloseFence();

    //
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

    static const uint32_t kMaxSRVCount_;

    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_; }

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_; }

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() { return commandQueue_; }

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSRV() { return srvDescriptorHeap_; }
    
    D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCPUHandle(uint32_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGPUHandle(uint32_t index);
    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCPUHandle(uint32_t index);

    uint32_t GetDescriptorSizeSRV() { return descriptorSizeSRV_; }
    uint32_t GetDescriptorSizeRTV() { return descriptorSizeRTV_; }
    uint32_t GetDescriptorSizeDSV() { return descriptorSizeDSV_; }
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

    // 各種デスクリプターヒープの生成
    void CreateDescriptorHeaps();

    // DescriptorsSizeを取得しておく
    uint32_t descriptorSizeSRV_;
    uint32_t descriptorSizeRTV_;
    uint32_t descriptorSizeDSV_;
    
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_; // RTV用のヒープでディスクリプタ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_; // SRV用のヒープでディスクリプタ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_; // DSV用のヒープでディスクリプタ

    // レンダーターゲットビューの生成
    void CreateRenderTargetView();
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]; // RTVを2つ作るのでディスクリプタを2つ用意
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];

    // ビューポート矩形の初期化
    void CreateViewportRect();
    D3D12_VIEWPORT viewport_{}; // ビューポート矩形
    
    // シーザー矩形の初期化
    void CreateScissorRect();
    D3D12_RECT scissorRect_{}; // シザー矩形

    // DXCの初期化
    void CreateShaderCompiler();
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

    // Imguiの初期化
    void CreateImgui();

    // DirectXの描画前処理
    UINT backBufferIndex_;
    D3D12_RESOURCE_BARRIER barrier_;
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;

    // FPS固定初期化
    void InitializeFixFPS();

    // FPS固定更新
    void UpdateFixFPS();

    // 記録時間(FPS固定用)
    std::chrono::steady_clock::time_point reference_;
};