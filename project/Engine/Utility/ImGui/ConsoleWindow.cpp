#include "ConsoleWindow.h"
#include <ctime>
#include <sstream>
#include <iomanip>

#ifdef USE_IMGUI

ConsoleWindow::ConsoleWindow()
    : isVisible_(true)
    , autoScroll_(true)
    , showInfo_(true)
    , showWarning_(true)
    , showError_(true)
    , showSuccess_(true) {
    memset(searchBuffer_, 0, sizeof(searchBuffer_));
}

void ConsoleWindow::Draw() {
    ImGui::Begin(GetWindowName(), &isVisible_);

    // ツールバー
    if (ImGui::Button("Clear")) {
        Clear();
    }
    ImGui::SameLine();
    
    // フィルターボタン
    ImGui::PushStyleColor(ImGuiCol_Button, showInfo_ ? ImVec4(0.2f, 0.2f, 0.2f, 1.0f) : ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
    if (ImGui::Button("Info")) {
        showInfo_ = !showInfo_;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, showWarning_ ? ImVec4(0.5f, 0.5f, 0.0f, 1.0f) : ImVec4(0.3f, 0.3f, 0.0f, 0.5f));
    if (ImGui::Button("Warning")) {
        showWarning_ = !showWarning_;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, showError_ ? ImVec4(0.5f, 0.0f, 0.0f, 1.0f) : ImVec4(0.3f, 0.0f, 0.0f, 0.5f));
    if (ImGui::Button("Error")) {
        showError_ = !showError_;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, showSuccess_ ? ImVec4(0.0f, 0.5f, 0.0f, 1.0f) : ImVec4(0.0f, 0.3f, 0.0f, 0.5f));
    if (ImGui::Button("Success")) {
        showSuccess_ = !showSuccess_;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &autoScroll_);
    
    // 検索バー
    ImGui::SameLine();
    ImGui::PushItemWidth(200);
    ImGuiHelper::SearchBar(searchBuffer_, sizeof(searchBuffer_), "Search logs...");
    ImGui::PopItemWidth();
    
    ImGui::Separator();

    // ログ表示領域
    ImGui::BeginChild("LogRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    
    for (const auto& log : logs_) {
        if (PassesFilter(log)) {
            ImGui::PushID(&log);
            
            // タイムスタンプ
            ImGui::TextDisabled("[%s]", log.timestamp.c_str());
            ImGui::SameLine();
            
            // ログレベルアイコン
            switch (log.level) {
                case ImGuiHelper::LogLevel::Info:
                    ImGui::Text("[I]");
                    break;
                case ImGuiHelper::LogLevel::Warning:
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[W]");
                    break;
                case ImGuiHelper::LogLevel::Error:
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "[E]");
                    break;
                case ImGuiHelper::LogLevel::Success:
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[S]");
                    break;
            }
            
            ImGui::SameLine();
            
            // メッセージ
            ImGuiHelper::ColoredText(log.message.c_str(), log.level);
            
            // 繰り返し回数
            if (log.count > 1) {
                ImGui::SameLine();
                ImGui::TextDisabled("(x%d)", log.count);
            }
            
            ImGui::PopID();
        }
    }
    
    // 自動スクロール
    if (autoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    ImGui::End();
}

void ConsoleWindow::AddLog(const std::string& message, ImGuiHelper::LogLevel level) {
    // 同じメッセージが連続する場合はカウントを増やす
    if (!logs_.empty() && logs_.back().message == message && logs_.back().level == level) {
        logs_.back().count++;
        return;
    }
    
    LogEntry entry;
    entry.message = message;
    entry.level = level;
    entry.timestamp = GetCurrentTimestamp();
    entry.count = 1;
    
    logs_.push_back(entry);
    
    // ログが多すぎる場合は古いものを削除（1000件まで保持）
    if (logs_.size() > 1000) {
        logs_.erase(logs_.begin());
    }
}

void ConsoleWindow::Clear() {
    logs_.clear();
}

ConsoleWindow* ConsoleWindow::GetInstance() {
    static ConsoleWindow instance;
    return &instance;
}

std::string ConsoleWindow::GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    std::tm tm_buf;
    localtime_s(&tm_buf, &time_t);
    ss << std::put_time(&tm_buf, "%H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

bool ConsoleWindow::PassesFilter(const LogEntry& entry) {
    // レベルフィルター
    switch (entry.level) {
        case ImGuiHelper::LogLevel::Info:
            if (!showInfo_) return false;
            break;
        case ImGuiHelper::LogLevel::Warning:
            if (!showWarning_) return false;
            break;
        case ImGuiHelper::LogLevel::Error:
            if (!showError_) return false;
            break;
        case ImGuiHelper::LogLevel::Success:
            if (!showSuccess_) return false;
            break;
    }
    
    // テキスト検索フィルター
    if (strlen(searchBuffer_) > 0) {
        if (entry.message.find(searchBuffer_) == std::string::npos) {
            return false;
        }
    }
    
    return true;
}

// グローバルログ関数の実装
namespace ImGuiLog {
    void Info(const std::string& message) {
        ConsoleWindow::GetInstance()->AddLog(message, ImGuiHelper::LogLevel::Info);
    }
    
    void Warning(const std::string& message) {
        ConsoleWindow::GetInstance()->AddLog(message, ImGuiHelper::LogLevel::Warning);
    }
    
    void Error(const std::string& message) {
        ConsoleWindow::GetInstance()->AddLog(message, ImGuiHelper::LogLevel::Error);
    }
    
    void Success(const std::string& message) {
        ConsoleWindow::GetInstance()->AddLog(message, ImGuiHelper::LogLevel::Success);
    }
}

#endif // USE_IMGUI
