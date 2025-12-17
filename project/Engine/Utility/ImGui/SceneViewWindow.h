#pragma once

#ifdef USE_IMGUI
#include "ImGuiManager.h"
#include <functional>

/// <summary>
/// Unityのシーンビュー/ゲームビュー風のウィンドウ
/// レンダーテクスチャを表示する
/// </summary>
class SceneViewWindow : public IImGuiWindow {
public:
    SceneViewWindow();
    ~SceneViewWindow() override = default;

    void Draw() override;
    const char* GetWindowName() const override { return "Scene View"; }
    bool IsVisible() const override { return isVisible_; }
    void SetVisible(bool visible) override { isVisible_ = visible; }

    /// <summary>
    /// 表示するテクスチャのSRVハンドルを設定
    /// </summary>
    void SetTexture(void* srv_gpu_handle);

    /// <summary>
    /// テクスチャサイズを設定
    /// </summary>
    void SetTextureSize(float width, float height) {
        textureWidth_ = width;
        textureHeight_ = height;
    }

    /// <summary>
    /// ウィンドウのアスペクト比を固定するか
    /// </summary>
    void SetFixedAspectRatio(bool fixed) { fixedAspectRatio_ = fixed; }

    /// <summary>
    /// 現在のウィンドウサイズを取得
    /// </summary>
    void GetWindowSize(float& width, float& height) const {
        width = currentWidth_;
        height = currentHeight_;
    }

private:
    bool isVisible_;
    void* textureHandle_;
    float textureWidth_;
    float textureHeight_;
    float currentWidth_;
    float currentHeight_;
    bool fixedAspectRatio_;
};

/// <summary>
/// ゲームビューウィンドウ（実行画面専用）
/// </summary>
class GameViewWindow : public IImGuiWindow {
public:
    GameViewWindow();
    ~GameViewWindow() override = default;

    void Draw() override;
    const char* GetWindowName() const override { return "Game View"; }
    bool IsVisible() const override { return isVisible_; }
    void SetVisible(bool visible) override { isVisible_ = visible; }

    /// <summary>
    /// 表示するテクスチャのSRVハンドルを設定
    /// </summary>
    void SetTexture(void* srv_gpu_handle);

    /// <summary>
    /// テクスチャサイズを設定
    /// </summary>
    void SetTextureSize(float width, float height) {
        textureWidth_ = width;
        textureHeight_ = height;
    }

    /// <summary>
    /// ウィンドウのアスペクト比を固定するか
    /// </summary>
    void SetFixedAspectRatio(bool fixed) { fixedAspectRatio_ = fixed; }

    /// <summary>
    /// 再生/一時停止コントロールを表示するか
    /// </summary>
    void SetShowControls(bool show) { showControls_ = show; }

    /// <summary>
    /// 統計情報を表示するか
    /// </summary>
    void SetShowStats(bool show) { showStats_ = show; }

    /// <summary>
    /// FPSを設定
    /// </summary>
    void SetFPS(float fps) { currentFPS_ = fps; }

private:
    bool isVisible_;
    void* textureHandle_;
    float textureWidth_;
    float textureHeight_;
    bool fixedAspectRatio_;
    bool showControls_;
    bool showStats_;
    float currentFPS_;
};

#endif // USE_IMGUI
