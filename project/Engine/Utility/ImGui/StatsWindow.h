#pragma once

#ifdef USE_IMGUI
#include "ImGuiManager.h"
#include <vector>
#include <string>

/// <summary>
/// パフォーマンス統計情報ウィンドウ
/// </summary>
class StatsWindow : public IImGuiWindow {
public:
    StatsWindow();
    ~StatsWindow() override = default;

    void Draw() override;
    const char* GetWindowName() const override { return "Stats"; }
    bool IsVisible() const override { return isVisible_; }
    void SetVisible(bool visible) override { isVisible_ = visible; }

    /// <summary>
    /// FPSを更新
    /// </summary>
    void UpdateFPS(float fps);

    /// <summary>
    /// フレーム時間を更新（ミリ秒）
    /// </summary>
    void UpdateFrameTime(float ms);

    /// <summary>
    /// 描画コール数を設定
    /// </summary>
    void SetDrawCalls(int count) { drawCalls_ = count; }

    /// <summary>
    /// 三角形数を設定
    /// </summary>
    void SetTriangleCount(int count) { triangleCount_ = count; }

    /// <summary>
    /// メモリ使用量を設定（MB）
    /// </summary>
    void SetMemoryUsage(float mb) { memoryUsageMB_ = mb; }

    /// <summary>
    /// カスタム統計情報を追加
    /// </summary>
    void AddCustomStat(const std::string& label, const std::string& value);

    /// <summary>
    /// カスタム統計情報をクリア
    /// </summary>
    void ClearCustomStats() { customStats_.clear(); }

private:
    bool isVisible_;
    
    // FPS履歴
    std::vector<float> fpsHistory_;
    float currentFPS_;
    float avgFPS_;
    float minFPS_;
    float maxFPS_;
    
    // フレーム時間履歴
    std::vector<float> frameTimeHistory_;
    float currentFrameTime_;
    
    // レンダリング統計
    int drawCalls_;
    int triangleCount_;
    float memoryUsageMB_;
    
    // カスタム統計
    struct CustomStat {
        std::string label;
        std::string value;
    };
    std::vector<CustomStat> customStats_;
    
    static constexpr int HISTORY_SIZE = 120; // 2秒分（60FPS想定）
};

#endif // USE_IMGUI
