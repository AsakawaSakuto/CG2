#include "RenderTexture.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/CreateResource/CreateResource.h"
#include <cassert>

void RenderTexture::Initialize(
    DirectXCommon* dxCommon,
    uint32_t width,
    uint32_t height,
    DXGI_FORMAT format,
    const float clearColor[4])
{
    assert(dxCommon);
    dxCommon_ = dxCommon;
    width_ = width;
    height_ = height;
    format_ = format;
    
    for (int i = 0; i < 4; ++i) {
        clearColor_[i] = clearColor[i];
    }

    // カラーテクスチャの作成
    // ResourceDesc 設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = width_;
    resourceDesc.Height = height_;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = format_;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // RTV用

    // Heap は DEFAULT VRAM
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // ClearValue
    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format_;
    clearValue.Color[0] = clearColor_[0];
    clearValue.Color[1] = clearColor_[1];
    clearValue.Color[2] = clearColor_[2];
    clearValue.Color[3] = clearColor_[3];

    // リソース作成
    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET, // 最初はRTVとして使う
        &clearValue,
        IID_PPV_ARGS(&resource_));
    assert(SUCCEEDED(hr));

    // 初期状態を記録
    currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

    // RTV作成
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = format_;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    // RTVヒープから取得、index 2を使用：SwapChainが0,1を使用しているため
    rtvHandle_ = ::GetCPUDescriptorHandle(
        dxCommon_->GetDescriptorHeapRTV().Get(),
        dxCommon_->GetDescriptorSizeRTV(),
        2);
    
    dxCommon_->GetDevice()->CreateRenderTargetView(
        resource_.Get(),
        &rtvDesc,
        rtvHandle_);

    // SRV作成（カラー用）
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format_;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    // SRVヒープからインデックス0を使用、ImGui用のインデックスの前
    uint32_t srvIndex = DirectXCommon::kMaxSRVCount_ - 2;
    srvCPUHandle_ = dxCommon_->GetSrvCPUHandle(srvIndex);
    srvGPUHandle_ = dxCommon_->GetSrvGPUHandle(srvIndex);

    dxCommon_->GetDevice()->CreateShaderResourceView(
        resource_.Get(),
        &srvDesc,
        srvCPUHandle_);

    // Depthテクスチャの作成
    // Depthリソースを作成
    depthResource_ = CreateDepthStencilTextureResource(dxCommon_->GetDevice().Get(), width_, height_);

    // DSV作成（Depth Stencil View）
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    // DSVハンドルを取得（index 1を使用：index 0はメインのDSV）
    dsvHandle_ = dxCommon_->GetDsvCPUHandle(1);
    
    dxCommon_->GetDevice()->CreateDepthStencilView(
        depthResource_.Get(),
        &dsvDesc,
        dsvHandle_);

    // SRV作成（Depth用 - Shader Read用）
    D3D12_SHADER_RESOURCE_VIEW_DESC depthSRVDesc{};
    depthSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // Depth部分だけ読む
    depthSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    depthSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    depthSRVDesc.Texture2D.MipLevels = 1;

    // Depth SRV用のインデックス
    uint32_t depthSRVIndex = DirectXCommon::kMaxSRVCount_ - 3;
    depthSRVCPUHandle_ = dxCommon_->GetSrvCPUHandle(depthSRVIndex);
    depthSRVGPUHandle_ = dxCommon_->GetSrvGPUHandle(depthSRVIndex);

    dxCommon_->GetDevice()->CreateShaderResourceView(
        depthResource_.Get(),
        &depthSRVDesc,
        depthSRVCPUHandle_);
}

void RenderTexture::SetAsRenderTarget(
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
    const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
{
    // dsvHandleが指定されていない場合は、RenderTexture専用のDSVを使用
    if (dsvHandle) {
        commandList->OMSetRenderTargets(1, &rtvHandle_, FALSE, dsvHandle);
    } else {
        commandList->OMSetRenderTargets(1, &rtvHandle_, FALSE, &dsvHandle_);
    }
}

void RenderTexture::Clear(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
    commandList->ClearRenderTargetView(rtvHandle_, clearColor_, 0, nullptr);
    // Depthもクリア
    commandList->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderTexture::TransitionToShaderResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
    // 既にShaderResourceならバリアは不要
    if (currentState_ == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
        return;
    }

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = currentState_;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);
    currentState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

void RenderTexture::TransitionToRenderTarget(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
    // 既にRenderTargetならバリアは不要
    if (currentState_ == D3D12_RESOURCE_STATE_RENDER_TARGET) {
        return;
    }

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = currentState_;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);
    currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
}
