#include "RenderTexture.h"
#include "../DirectXCommon/DirectXCommon.h"
#include "../Utility/CreateResource/CreateResource.h"
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

    // Heap は DEFAULT（VRAM）
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

    // RTV作成
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = format_;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    // RTVヒープから取得（index 2を使用：SwapChainが0,1を使用しているため）
    rtvHandle_ = ::GetCPUDescriptorHandle(
        dxCommon_->GetDescriptorHeapRTV().Get(),
        dxCommon_->GetDescriptorSizeRTV(),
        2);
    
    dxCommon_->GetDevice()->CreateRenderTargetView(
        resource_.Get(),
        &rtvDesc,
        rtvHandle_);

    // SRV作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format_;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    // SRVヒープからインデックス0を使用（ImGui用のインデックスの前）
    uint32_t srvIndex = DirectXCommon::kMaxSRVCount_ - 2;
    srvCPUHandle_ = dxCommon_->GetSrvCPUHandle(srvIndex);
    srvGPUHandle_ = dxCommon_->GetSrvGPUHandle(srvIndex);

    dxCommon_->GetDevice()->CreateShaderResourceView(
        resource_.Get(),
        &srvDesc,
        srvCPUHandle_);
}

void RenderTexture::SetAsRenderTarget(
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
    const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
{
    commandList->OMSetRenderTargets(1, &rtvHandle_, FALSE, dsvHandle);
}

void RenderTexture::Clear(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
    commandList->ClearRenderTargetView(rtvHandle_, clearColor_, 0, nullptr);
}

void RenderTexture::TransitionToShaderResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);
}

void RenderTexture::TransitionToRenderTarget(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource_.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);
}
