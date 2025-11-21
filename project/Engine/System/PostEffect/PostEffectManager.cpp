#include "PostEffectManager.h"
#include "DirectXCommon.h"
#include "CreateResource.h"
#include "../Engine/System/PSOManager/PSOManager.h"
#include <cassert>

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif

void PostEffectManager::Initialize(DirectXCommon* dxCommon, uint32_t width, uint32_t height) {
    assert(dxCommon);
    dxCommon_ = dxCommon;

    // RenderTextureの初期化
    renderTexture_ = std::make_unique<RenderTexture>();
    float clearColor[4] = { 0.15f, 0.15f, 0.15f, 1.0f };
    renderTexture_->Initialize(dxCommon_, width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);

    // パラメータ用の定数バッファを作成
    CreateParamResource();
}

void PostEffectManager::BeginOffscreenRendering() {
    if (!enabled_ || !renderTexture_) {
        return;
    }

    auto commandList = dxCommon_->GetCommandList();

    // RenderTextureをRenderTarget状態に遷移
    renderTexture_->TransitionToRenderTarget(commandList);

    // RenderTargetとDepthStencilを設定
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxCommon_->GetDsvCPUHandle(0);
    renderTexture_->SetAsRenderTarget(commandList, &dsvHandle);

    // クリア
    renderTexture_->Clear(commandList);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffectManager::EndOffscreenRenderingAndApplyEffect(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle) {
    if (!enabled_ || !renderTexture_) {
        return;
    }

    auto commandList = dxCommon_->GetCommandList();

    // RenderTextureをShaderResource状態に遷移
    renderTexture_->TransitionToShaderResource(commandList);

    // SwapChainのRTVを設定（Depth無し)
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // 背景クリア（念のため）
    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // パラメータを更新
    UpdateParams();

    // PSOManagerからRootSignatureとPSOを取得
    auto& psoManager = PSOManager::GetInstance();
    auto rootSignature = psoManager.GetRootSignature("Offscreen");
    auto pso = psoManager.GetPSO(effectType_);

    // ルートシグネチャとPSOを設定
    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->SetPipelineState(pso.Get());

    // テクスチャとパラメータを設定
    commandList->SetGraphicsRootDescriptorTable(0, renderTexture_->GetSRVGPUHandle());
    commandList->SetGraphicsRootConstantBufferView(1, paramResource_->GetGPUVirtualAddress());

    // フルスクリーン三角形を描画
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawInstanced(3, 1, 0, 0);
}

void PostEffectManager::Resize(uint32_t width, uint32_t height) {
    if (!renderTexture_) {
        return;
    }

    // RenderTextureを再作成
    renderTexture_ = std::make_unique<RenderTexture>();
    float clearColor[4] = { 0.15f, 0.15f, 0.15f, 1.0f };
    renderTexture_->Initialize(dxCommon_, width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
}

void PostEffectManager::CreateParamResource() {
    // パラメータ用の定数バッファを作成
    paramResource_ = CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(PostEffectParams));
}

void PostEffectManager::UpdateParams() {
    if (!paramResource_) {
        return;
    }

    // 現在のエフェクトタイプに応じてパラメータを定数バッファにコピー
    void* mappedData = nullptr;
    paramResource_->Map(0, nullptr, &mappedData);

    switch (effectType_) {
        case PSOType::PostEffect_Vignette:
            memcpy(mappedData, &params_.vignette, sizeof(VignetteParams));
            break;
        case PSOType::PostEffect_Blur:
            memcpy(mappedData, &params_.blur, sizeof(BlurParams));
            break;
        case PSOType::PostEffect_Sepia:
            memcpy(mappedData, &params_.sepia, sizeof(SepiaParams));
            break;
        case PSOType::PostEffect_Grayscale:
            memcpy(mappedData, &params_.grayscale, sizeof(GrayscaleParams));
            break;
        default:
            // その他のエフェクトはパラメータ不要
            break;
    }

    paramResource_->Unmap(0, nullptr);
}

#ifdef USE_IMGUI
void PostEffectManager::DrawImGui() {
    ImGui::Begin("Post Effect Settings");

    // オフスクリーンレンダリングのON/OFF
    bool useOffscreen = IsEnabled();
    if (ImGui::Checkbox("Enable Post Effects", &useOffscreen)) {
        SetEnabled(useOffscreen);
    }

    ImGui::Separator();

    // ポストエフェクトタイプの選択
    const char* effectNames[] = {
        "None (通常)",
        "Grayscale (白黒)",
        "Sepia (セピア調)",
        "Vignette (周辺減光)",
        "Invert (色反転)",
        "Blur (ぼかし)"
    };

    int currentEffect = static_cast<int>(effectType_);
    int baseOffset = static_cast<int>(PSOType::PostEffect_None);
    int selectedIndex = currentEffect - baseOffset;

    if (selectedIndex >= 0 && selectedIndex < IM_ARRAYSIZE(effectNames)) {
        if (ImGui::Combo("Effect Type", &selectedIndex, effectNames, IM_ARRAYSIZE(effectNames))) {
            PSOType newEffect = static_cast<PSOType>(baseOffset + selectedIndex);
            SetPostEffectType(newEffect);
        }
    }

    ImGui::Separator();
    ImGui::Text("Effect Parameters");
    ImGui::Separator();

    // 各エフェクトのパラメータ調整
    switch (effectType_) {
        case PSOType::PostEffect_Grayscale:
        {
            ImGui::Text("Grayscale Settings");
            ImGui::SliderFloat("Intensity", &params_.grayscale.intensity, 0.0f, 1.0f);
            if (ImGui::Button("Reset##Grayscale")) {
                params_.grayscale.intensity = 1.0f;
            }
            ImGui::TextWrapped("Intensity: グレースケールの強度を調整します。\n0.0 = カラー, 1.0 = 完全な白黒");
            break;
        }

        case PSOType::PostEffect_Sepia:
        {
            ImGui::Text("Sepia Settings");
            ImGui::SliderFloat("Intensity", &params_.sepia.intensity, 0.0f, 1.0f);
            if (ImGui::Button("Reset##Sepia")) {
                params_.sepia.intensity = 1.0f;
            }
            ImGui::TextWrapped("Intensity: セピア調の強度を調整します。\n0.0 = カラー, 1.0 = 完全なセピア");
            break;
        }

        case PSOType::PostEffect_Vignette:
        {
            ImGui::Text("Vignette Settings");
            ImGui::SliderFloat("Strength", &params_.vignette.strength, 0.0f, 1.0f);
            ImGui::SliderFloat("Radius", &params_.vignette.radius, 0.0f, 1.0f);
            ImGui::SliderFloat("Smoothness", &params_.vignette.smoothness, 1.0f, 5.0f);
            if (ImGui::Button("Reset##Vignette")) {
                params_.vignette.strength = 0.8f;
                params_.vignette.radius = 0.7f;
                params_.vignette.smoothness = 2.0f;
            }
            ImGui::TextWrapped(
                "Strength: 周辺減光の強さ\n"
                "Radius: ビネット効果の開始位置\n"
                "Smoothness: エッジの滑らかさ"
            );
            break;
        }

        case PSOType::PostEffect_Blur:
        {
            ImGui::Text("Blur Settings");
            ImGui::SliderFloat("Amount", &params_.blur.amount, 0.0f, 5.0f);

            const char* sampleItems[] = { "9 Samples (Fast)", "25 Samples (Quality)" };
            int currentSample = (params_.blur.sampleCount == 9) ? 0 : 1;
            if (ImGui::Combo("Quality", &currentSample, sampleItems, IM_ARRAYSIZE(sampleItems))) {
                params_.blur.sampleCount = (currentSample == 0) ? 9 : 25;
            }

            if (ImGui::Button("Reset##Blur")) {
                params_.blur.amount = 1.0f;
                params_.blur.sampleCount = 9;
            }
            ImGui::TextWrapped(
                "Amount: ぼかしの強さ\n"
                "Quality: サンプル数（多いほど綺麗だが重い）"
            );
            break;
        }

        case PSOType::PostEffect_Invert:
        {
            ImGui::Text("Invert Settings");
            ImGui::TextWrapped("色反転エフェクトにはパラメータはありません。");
            break;
        }

        case PSOType::PostEffect_None:
        {
            ImGui::Text("No Effect Selected");
            ImGui::TextWrapped("エフェクトが選択されていません。");
            break;
        }

        default:
            ImGui::Text("Unknown Effect");
            break;
    }

    ImGui::Separator();

    // 統計情報
    if (ImGui::CollapsingHeader("Statistics")) {
        ImGui::Text("Render Texture: %s", enabled_ ? "Enabled" : "Disabled");
        ImGui::Text("Current Effect: %s", effectNames[selectedIndex]);
        if (renderTexture_) {
            ImGui::Text("Texture Size: %u x %u", renderTexture_->GetWidth(), renderTexture_->GetHeight());
        }
    }

    // 全パラメータリセット
    ImGui::Separator();
    if (ImGui::Button("Reset All Parameters")) {
        params_.grayscale.intensity = 1.0f;
        params_.sepia.intensity = 1.0f;
        params_.vignette.strength = 0.8f;
        params_.vignette.radius = 0.7f;
        params_.vignette.smoothness = 2.0f;
        params_.blur.amount = 1.0f;
        params_.blur.sampleCount = 9;
    }

    ImGui::End();
}
#endif
