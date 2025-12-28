#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include "Math/Type/Matrix4x4.h"
#include "Math/Type/Vector3.h"

/// <summary>
/// シャドウマップ管理クラス
/// Directional Light用のシャドウマッピングを実装
/// </summary>
class ShadowMap {
public:
    ShadowMap() = default;
    ~ShadowMap() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    /// <param name="width">シャドウマップの幅</param>
    /// <param name="height">シャドウマップの高さ</param>
    void Initialize(ID3D12Device* device, uint32_t width = 2048, uint32_t height = 2048);

    /// <summary>
    /// ライト空間の行列を更新
    /// </summary>
    /// <param name="lightDirection">ライト方向（正規化済み）</param>
    /// <param name="sceneCenter">シーンの中心座標</param>
    /// <param name="sceneRadius">シーンの半径</param>
    void UpdateLightSpaceMatrix(const Vector3& lightDirection, const Vector3& sceneCenter = {0, 0, 0}, float sceneRadius = 50.0f);

    /// <summary>
    /// 描画開始（DSVをセット）
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void BeginShadowPass(ID3D12GraphicsCommandList* commandList);

    /// <summary>
    /// 描画終了（バリア設定）
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void EndShadowPass(ID3D12GraphicsCommandList* commandList);

    // Getter
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const { return dsvHandle_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const { return srvHandle_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPU() const { return srvHandleGPU_; }
    const Matrix4x4& GetLightSpaceMatrix() const { return lightSpaceMatrix_; }
    uint32_t GetWidth() const { return width_; }
    uint32_t GetHeight() const { return height_; }

private:
    void CreateDepthTexture(ID3D12Device* device);
    void CreateDSV(ID3D12Device* device);
    void CreateSRV(ID3D12Device* device);

private:
    // リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> depthTexture_;
    
    // デスクリプタハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_{};
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_{};

    // シャドウマップのサイズ
    uint32_t width_ = 2048;
    uint32_t height_ = 2048;

    // ライト空間変換行列
    Matrix4x4 lightSpaceMatrix_;
    Matrix4x4 lightView_;
    Matrix4x4 lightProjection_;

    // ライト設定
    Vector3 lightDirection_{0.0f, -1.0f, 0.0f};
    float orthoSize_ = 50.0f;  // 正射影のサイズ
    float nearPlane_ = 0.1f;
    float farPlane_ = 100.0f;
};
