#pragma once

#ifdef USE_IMGUI
#include "ImGuiManager.h"
#include "ImGuiHelper.h"
#include <vector>
#include <string>
#include <chrono>

/// <summary>
/// Unityのコンソール風のログウィンドウ
/// </summary>
class ConsoleWindow : public IImGuiWindow {
public:
    struct LogEntry {
        std::string message;
        ImGuiHelper::LogLevel level;
        std::string timestamp;
        int count; // 同じメッセージの繰り返し回数
    };

    ConsoleWindow();
    ~ConsoleWindow() override = default;

    void Draw() override;
    const char* GetWindowName() const override { return "Console"; }
    bool IsVisible() const override { return isVisible_; }
    void SetVisible(bool visible) override { isVisible_ = visible; }

    /// <summary>
    /// ログを追加
    /// </summary>
    void AddLog(const std::string& message, ImGuiHelper::LogLevel level = ImGuiHelper::LogLevel::Info);

    /// <summary>
    /// すべてのログをクリア
    /// </summary>
    void Clear();

    /// <summary>
    /// 静的アクセス用のインスタンス取得
    /// </summary>
    static ConsoleWindow* GetInstance();

private:
    bool isVisible_;
    std::vector<LogEntry> logs_;
    bool autoScroll_;
    bool showInfo_;
    bool showWarning_;
    bool showError_;
    bool showSuccess_;
    char searchBuffer_[256];
    
    std::string GetCurrentTimestamp();
    bool PassesFilter(const LogEntry& entry);
};

// グローバルログ関数
namespace ImGuiLog {
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Success(const std::string& message);
}

#endif // USE_IMGUI
