#include "ShadowMap.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/ServiceLocator/ServiceLocator.h"
#include "Math/Type/Vector3.h"
#include "Math/MatrixFunction/MatrixFunction.h"
#include <stdexcept>

void ShadowMap::Initialize(ID3D12Device* device, uint32_t width, uint32_t height) {
    width_ = width;
    height_ = height;

    // 深度テクスチャの作成
    CreateDepthTexture(device);
    
    // DSVの作成
    CreateDSV(device);
    
    // SRVの作成
    CreateSRV(device);
}

void ShadowMap::CreateDepthTexture(ID3D12Device* device) {
    // ヒーププロパティ
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;  // VRAM上に配置
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    // リソースデスク
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Width = width_;
    resourceDesc.Height = height_;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;  // DSVとSRVで異なるフォーマットを使うためTypeless
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;  // 深度ステンシルとして使用
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    // クリア値
    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;  // 深度フォーマット
    clearValue.DepthStencil.Depth = 1.0f;      // 最大深度でクリア
    clearValue.DepthStencil.Stencil = 0;

    // リソース作成
    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,  // 初期状態は深度書き込み
        &clearValue,
        IID_PPV_ARGS(&depthTexture_)
    );

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create shadow map depth texture");
    }

    depthTexture_->SetName(L"ShadowMap_DepthTexture");
}

void ShadowMap::CreateDSV(ID3D12Device* device) {
    // DirectXCommonからDSVヒープを取得
    DirectXCommon* dxCommon = ServiceLocator::GetDXCommon();
    
    // DSVハンドルを割り当て
    // DirectXCommonのDSVヒープのインデックス1を使用（0はメインのDepthBuffer用）
    dsvHandle_ = dxCommon->GetDsvCPUHandle(1);
    
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.Texture2D.MipSlice = 0;

    // DSVを作成
    device->CreateDepthStencilView(depthTexture_.Get(), &dsvDesc, dsvHandle_);
}

void ShadowMap::CreateSRV(ID3D12Device* device) {
    // DirectXCommonからSRVヒープを取得
    DirectXCommon* dxCommon = ServiceLocator::GetDXCommon();
    auto srvHeap = dxCommon->GetSRV();
    uint32_t descriptorSize = dxCommon->GetDescriptorSizeSRV();

    // SRV用のインデックスを取得（実装に応じて調整）
    // TODO: DirectXCommonにSRVインデックス管理機能を追加
    static uint32_t srvIndex = 100;  // 仮の値、実際は動的に割り当て
    
    srvHandle_ = dxCommon->GetCPUDescriptorHandle(srvHeap, descriptorSize, srvIndex);
    srvHandleGPU_ = dxCommon->GetGPUDescriptorHandle(srvHeap, descriptorSize, srvIndex);

    // SRVデスク
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;  // 深度値を浮動小数点として読む
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.PlaneSlice = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    device->CreateShaderResourceView(depthTexture_.Get(), &srvDesc, srvHandle_);
}

void ShadowMap::UpdateLightSpaceMatrix(const Vector3& lightDirection, const Vector3& sceneCenter, float sceneRadius) {
    lightDirection_ = lightDirection.Normalized();

    // ライトの位置を計算（シーンの中心からライト方向の逆方向に配置）
    Vector3 lightPos = sceneCenter - lightDirection_ * sceneRadius;

    // ビュー行列（ライトから見た視点）
    // 既存のカメラ実装に合わせて、アフィン行列の逆行列を使用
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    // ライト方向がほぼ上向きの場合は別の上ベクトルを使用
    if (std::abs(Vector3::Dot(lightDirection_, up)) > 0.99f) {
        up = Vector3(0.0f, 0.0f, 1.0f);
    }

    // ライトから見たターゲット方向を計算
    Vector3 forward = lightDirection_;
    Vector3 right = Vector3::Cross(up, forward).Normalized();
    up = Vector3::Cross(forward, right).Normalized();

    // ライトカメラのワールド行列を構築
    Matrix4x4 lightCameraMatrix = MakeIdentityMatrix();
    
    // 回転部分（右、上、前方）
    lightCameraMatrix.m[0][0] = right.x;
    lightCameraMatrix.m[0][1] = right.y;
    lightCameraMatrix.m[0][2] = right.z;
    
    lightCameraMatrix.m[1][0] = up.x;
    lightCameraMatrix.m[1][1] = up.y;
    lightCameraMatrix.m[1][2] = up.z;
    
    lightCameraMatrix.m[2][0] = forward.x;
    lightCameraMatrix.m[2][1] = forward.y;
    lightCameraMatrix.m[2][2] = forward.z;
    
    // 平行移動部分
    lightCameraMatrix.m[3][0] = lightPos.x;
    lightCameraMatrix.m[3][1] = lightPos.y;
    lightCameraMatrix.m[3][2] = lightPos.z;

    // ビュー行列は逆行列
    lightView_ = InverseMatrix(lightCameraMatrix);

    // 正射影行列（Directional Lightなので平行投影）
    orthoSize_ = sceneRadius * 2.0f;
    lightProjection_ = MakeOrthographicMatrix(
        -orthoSize_, orthoSize_,  // left, right
        -orthoSize_, orthoSize_,  // top, bottom
        nearPlane_, farPlane_     // near, far
    );

    // ライト空間変換行列（row-major の場合）
    lightSpaceMatrix_ = lightView_ * lightProjection_;
}

void ShadowMap::BeginShadowPass(ID3D12GraphicsCommandList* commandList) {
    // リソースバリア: DEPTH_WRITE状態に遷移
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = depthTexture_.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    
    commandList->ResourceBarrier(1, &barrier);

    // ビューポート設定
    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(width_);
    viewport.Height = static_cast<float>(height_);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    commandList->RSSetViewports(1, &viewport);

    // シザー矩形設定
    D3D12_RECT scissorRect{};
    scissorRect.left = 0;
    scissorRect.top = 0;
    scissorRect.right = width_;
    scissorRect.bottom = height_;
    commandList->RSSetScissorRects(1, &scissorRect);

    // DSVをセット（RTVはnullptr）
    commandList->OMSetRenderTargets(0, nullptr, FALSE, &dsvHandle_);

    // 深度バッファをクリア
    commandList->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void ShadowMap::EndShadowPass(ID3D12GraphicsCommandList* commandList) {
    // リソースバリア: PIXEL_SHADER_RESOURCE状態に遷移（次のパスで読み取るため）
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = depthTexture_.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    
    commandList->ResourceBarrier(1, &barrier);
}
