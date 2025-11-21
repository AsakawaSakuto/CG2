#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>

class DirectXCommon;

/// <summary>
/// オフスクリーンレンダリング用のレンダーターゲットテクスチャ
/// </summary>
class RenderTexture
{
public:
    RenderTexture() = default;
    ~RenderTexture() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommonのポインタ</param>
    /// <param name="width">テクスチャの幅</param>
    /// <param name="height">テクスチャの高さ</param>
    /// <param name="format">テクスチャフォーマット</param>
    /// <param name="clearColor">クリアカラー（RGBA）</param>
    void Initialize(
        DirectXCommon* dxCommon,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format,
        const float clearColor[4]);

    /// <summary>
    /// レンダーターゲットとして設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="dsvHandle">深度ステンシルビューのハンドル（オプション）</param>
    void SetAsRenderTarget(
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
        const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr);

    /// <summary>
    /// クリア
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void Clear(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

    /// <summary>
    /// リソースバリア：RenderTarget → ShaderResource
    /// </summary>
    void TransitionToShaderResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

    /// <summary>
    /// リソースバリア：ShaderResource → RenderTarget
    /// </summary>
    void TransitionToRenderTarget(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

    // Getter
    Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const { return resource_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const { return rtvHandle_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle() const { return srvCPUHandle_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const { return srvGPUHandle_; }
    uint32_t GetWidth() const { return width_; }
    uint32_t GetHeight() const { return height_; }

private:
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{};
    D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle_{};
    D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle_{};
    
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
    float clearColor_[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};
