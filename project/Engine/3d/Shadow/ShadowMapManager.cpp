#include "ShadowMapManager.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/ServiceLocator/ServiceLocator.h"

ShadowMapManager* ShadowMapManager::GetInstance() {
    static ShadowMapManager instance;
    return &instance;
}

void ShadowMapManager::Initialize() {
    DirectXCommon* dxCommon = ServiceLocator::GetDXCommon();
    
    shadowMap_ = std::make_unique<ShadowMap>();
    shadowMap_->Initialize(dxCommon->GetDevice().Get(), 2048, 2048);
}

void ShadowMapManager::Finalize() {
    shadowMap_.reset();
}

void ShadowMapManager::UpdateLightSpace(const Vector3& lightDirection, const Vector3& sceneCenter, float sceneRadius) {
    shadowMap_->UpdateLightSpaceMatrix(lightDirection, sceneCenter, sceneRadius);
}

void ShadowMapManager::RenderShadowMap(std::function<void()> renderCallback) {
    DirectXCommon* dxCommon = ServiceLocator::GetDXCommon();
    auto commandList = dxCommon->GetCommandList();

    // シャドウマップ描画開始
    shadowMap_->BeginShadowPass(commandList.Get());

    // TODO: シャドウマップ用PSO（Pipeline State Object）をセット
    // - RenderTargetなし（RTVCount = 0）
    // - DepthStencilのみ有効
    // - CullMode = FRONT（Peter Panning対策）
    // - DepthBias設定（ハードウェアバイアス）
    
    // シーンを描画（ライト視点）
    if (renderCallback) {
        renderCallback();
    }

    // シャドウマップ描画終了
    shadowMap_->EndShadowPass(commandList.Get());
}

void ShadowMapManager::RenderScene(Camera& camera, std::function<void()> renderCallback) {
    DirectXCommon* dxCommon = ServiceLocator::GetDXCommon();
    auto commandList = dxCommon->GetCommandList();

    // TODO: 通常描画用PSOをセット
    // - RenderTarget有効
    // - DepthStencil有効
    // - CullMode = BACK
    // - シャドウマップをSRVとしてバインド
    
    // 通常描画（カメラ視点）
    if (renderCallback) {
        renderCallback();
    }
}
