#include "StatsWindow.h"
#include "ImGuiHelper.h"
#include <algorithm>
#include <numeric>

#ifdef USE_IMGUI

StatsWindow::StatsWindow()
    : isVisible_(true)
    , currentFPS_(0.0f)
    , avgFPS_(0.0f)
    , minFPS_(0.0f)
    , maxFPS_(0.0f)
    , currentFrameTime_(0.0f)
    , drawCalls_(0)
    , triangleCount_(0)
    , memoryUsageMB_(0.0f) {
    fpsHistory_.reserve(HISTORY_SIZE);
    frameTimeHistory_.reserve(HISTORY_SIZE);
}

void StatsWindow::Draw() {
    ImGui::Begin(GetWindowName(), &isVisible_);

    // FPSセクション
    if (ImGuiHelper::BeginPropertySection("Performance", true)) {
        ImGui::Text("FPS: %.1f", currentFPS_);
        ImGui::SameLine();
        ImGuiHelper::HelpMarker("Frames Per Second");
        
        ImGui::Text("Avg: %.1f | Min: %.1f | Max: %.1f", avgFPS_, minFPS_, maxFPS_);
        
        // FPSグラフ
        if (!fpsHistory_.empty()) {
            ImGui::PlotLines("##FPS", fpsHistory_.data(), static_cast<int>(fpsHistory_.size()), 
                           0, nullptr, 0.0f, maxFPS_ * 1.2f, ImVec2(0, 80));
        }
        
        ImGuiHelper::StyledSeparator();
        
        // フレーム時間
        ImGui::Text("Frame Time: %.2f ms", currentFrameTime_);
        
        if (!frameTimeHistory_.empty()) {
            ImGui::PlotHistogram("##FrameTime", frameTimeHistory_.data(), 
                               static_cast<int>(frameTimeHistory_.size()), 
                               0, nullptr, 0.0f, 33.33f, ImVec2(0, 80));
        }
        
        ImGuiHelper::EndPropertySection();
    }

    // レンダリング統計
    if (ImGuiHelper::BeginPropertySection("Rendering", true)) {
        ImGuiHelper::ReadOnlyInt("Draw Calls", drawCalls_);
        ImGuiHelper::ReadOnlyInt("Triangles", triangleCount_);
        
        // 三角形数の視覚化
        if (triangleCount_ > 0) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d tris", triangleCount_);
            float fraction = (std::min)(triangleCount_ / 100000.0f, 1.0f);
            ImGuiHelper::ProgressBar(fraction, buf);
        }
        
        ImGuiHelper::EndPropertySection();
    }

    // メモリ
    if (ImGuiHelper::BeginPropertySection("Memory", false)) {
        ImGui::Text("Usage: %.2f MB", memoryUsageMB_);
        
        // メモリ使用量バー
        float memoryFraction = (std::min)(memoryUsageMB_ / 1024.0f, 1.0f); // 1GBを上限として
        char memBuf[32];
        snprintf(memBuf, sizeof(memBuf), "%.0f MB", memoryUsageMB_);
        ImGuiHelper::ProgressBar(memoryFraction, memBuf);
        
        ImGuiHelper::EndPropertySection();
    }

    // カスタム統計
    if (!customStats_.empty()) {
        if (ImGuiHelper::BeginPropertySection("Custom Stats", false)) {
            for (const auto& stat : customStats_) {
                ImGuiHelper::ReadOnlyText(stat.label.c_str(), stat.value.c_str());
            }
            ImGuiHelper::EndPropertySection();
        }
    }

    // システム情報
    if (ImGuiHelper::BeginPropertySection("System", false)) {
        ImGui::Text("ImGui: %s", ImGui::GetVersion());
        ImGui::Text("Windows: %d", ImGui::GetIO().MetricsActiveWindows);
        ImGuiHelper::EndPropertySection();
    }

    ImGui::End();
}

void StatsWindow::UpdateFPS(float fps) {
    currentFPS_ = fps;
    
    // 履歴に追加
    fpsHistory_.push_back(fps);
    if (fpsHistory_.size() > HISTORY_SIZE) {
        fpsHistory_.erase(fpsHistory_.begin());
    }
    
    // 統計を計算
    if (!fpsHistory_.empty()) {
        avgFPS_ = std::accumulate(fpsHistory_.begin(), fpsHistory_.end(), 0.0f) / fpsHistory_.size();
        minFPS_ = *std::min_element(fpsHistory_.begin(), fpsHistory_.end());
        maxFPS_ = *std::max_element(fpsHistory_.begin(), fpsHistory_.end());
    }
}

void StatsWindow::UpdateFrameTime(float ms) {
    currentFrameTime_ = ms;
    
    // 履歴に追加
    frameTimeHistory_.push_back(ms);
    if (frameTimeHistory_.size() > HISTORY_SIZE) {
        frameTimeHistory_.erase(frameTimeHistory_.begin());
    }
}

void StatsWindow::AddCustomStat(const std::string& label, const std::string& value) {
    // 既存の統計を更新または新規追加
    for (auto& stat : customStats_) {
        if (stat.label == label) {
            stat.value = value;
            return;
        }
    }
    customStats_.push_back({label, value});
}

#endif // USE_IMGUI
