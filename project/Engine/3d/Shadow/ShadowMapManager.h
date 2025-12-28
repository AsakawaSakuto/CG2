#pragma once
#include "ShadowMap.h"
#include "Camera/Camera.h"
#include <memory>
#include <functional>

/// <summary>
/// シャドウマッピング統括管理クラス
/// 描画パスの切り替えとリソース管理を行う
/// </summary>
class ShadowMapManager {
public:
    static ShadowMapManager* GetInstance();

    void Initialize();
    void Finalize();

    /// <summary>
    /// シャドウマップの更新
    /// </summary>
    /// <param name="lightDirection">ライト方向</param>
    /// <param name="sceneCenter">シーンの中心</param>
    /// <param name="sceneRadius">シーンの半径</param>
    void UpdateLightSpace(const Vector3& lightDirection, const Vector3& sceneCenter = {0, 0, 0}, float sceneRadius = 50.0f);

    /// <summary>
    /// シャドウマップ生成パス（1st Pass）
    /// </summary>
    /// <param name="renderCallback">シーンを描画するコールバック関数</param>
    void RenderShadowMap(std::function<void()> renderCallback);

    /// <summary>
    /// 通常描画パス（2nd Pass）
    /// </summary>
    /// <param name="camera">カメラ</param>
    /// <param name="renderCallback">シーンを描画するコールバック関数</param>
    void RenderScene(Camera& camera, std::function<void()> renderCallback);

    // Getter
    ShadowMap* GetShadowMap() const { return shadowMap_.get(); }
    const Matrix4x4& GetLightSpaceMatrix() const { return shadowMap_->GetLightSpaceMatrix(); }

    // シャドウパラメータ
    void SetShadowBias(float bias) { shadowBias_ = bias; }
    void SetShadowBiasSlope(float slope) { shadowBiasSlope_ = slope; }
    float GetShadowBias() const { return shadowBias_; }
    float GetShadowBiasSlope() const { return shadowBiasSlope_; }

private:
    ShadowMapManager() = default;
    ~ShadowMapManager() = default;
    ShadowMapManager(const ShadowMapManager&) = delete;
    ShadowMapManager& operator=(const ShadowMapManager&) = delete;

private:
    std::unique_ptr<ShadowMap> shadowMap_;
    
    // シャドウパラメータ
    float shadowBias_ = 0.005f;         // 基本バイアス
    float shadowBiasSlope_ = 0.05f;     // スロープスケールバイアス
};
