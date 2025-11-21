#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <memory>
#include "../PSOManager/PSOType.h"
#include "../PostEffect/PostEffectParams.h"
#include "../RenderTexture/RenderTexture.h"
#include "Matrix4x4.h"
#include "MatrixFunction.h"


class DirectXCommon;

/// <summary>
/// ポストエフェクトマネージャー
/// オフスクリーンレンダリングとポストエフェクトの適用を管理
/// </summary>
class PostEffectManager
{
public:
    PostEffectManager() = default;
    ~PostEffectManager() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommonのポインタ</param>
    /// <param name="width">レンダーテクスチャの幅</param>
    /// <param name="height">レンダーテクスチャの高さ</param>
    void Initialize(DirectXCommon* dxCommon, uint32_t width, uint32_t height);

    /// <summary>
    /// オフスクリーンレンダリング開始、RenderTextureへの描画を開始
    /// </summary>
    void BeginOffscreenRendering();

    /// <summary>
    /// オフスクリーンレンダリング終了とポストエフェクト適用
    /// RenderTextureの内容をSwapChainにエフェクトを適用して描画
    /// </summary>
    /// <param name="rtvHandle">SwapChainのRTVハンドル</param>
    void EndOffscreenRenderingAndApplyEffect(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);

    /// <summary>
    /// リサイズ対応
    /// </summary>
    /// <param name="width">新しい幅</param>
    /// <param name="height">新しい高さ</param>
    void Resize(uint32_t width, uint32_t height);

    // Getter/Setter
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    bool IsEnabled() const { return enabled_; }

    void SetPostEffectType(PSOType effectType) { effectType_ = effectType; }
    PSOType GetPostEffectType() const { return effectType_; }

    PostEffectParams& GetParams() { return params_; }
    const PostEffectParams& GetParams() const { return params_; }

    RenderTexture* GetRenderTexture() const { return renderTexture_.get(); }

    /// <summary>
    /// カメラの投影行列を設定、Depthベースエフェクト用
    /// </summary>
    /// <param name="projectionMatrix">投影行列</param>
    void SetProjectionMatrix(const Matrix4x4& projectionMatrix) { projectionMatrix_ = projectionMatrix; }

#ifdef USE_IMGUI
    /// <summary>
    /// ImGuiでポストエフェクト設定を表示
    /// </summary>
    void DrawImGui();
#endif

private:
    DirectXCommon* dxCommon_ = nullptr;
    
    // オフスクリーンレンダリング用のレンダーテクスチャ
    std::unique_ptr<RenderTexture> renderTexture_;
    
    // ポストエフェクト有効フラグ
    bool enabled_ = true;
    
    // 現在のエフェクトタイプ
    PSOType effectType_ = PSOType::PostEffect_None;
    
    // エフェクトパラメータ
    PostEffectParams params_;
    
    // パラメータ用の定数バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> paramResource_;
    
    // カメラの投影行列（Depthベースエフェクト用）
    Matrix4x4 projectionMatrix_ = MakeIdentityMatrix();
    
    /// <summary>
    /// パラメータ用の定数バッファを作成
    /// </summary>
    void CreateParamResource();
    
    /// <summary>
    /// パラメータを更新
    /// </summary>
    void UpdateParams();
};
