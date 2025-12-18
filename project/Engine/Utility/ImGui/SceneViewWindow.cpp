#include "SceneViewWindow.h"
#include "ImGuiHelper.h"

#ifdef USE_IMGUI

// SceneViewWindow の実装
SceneViewWindow::SceneViewWindow()
    : isVisible_(true)
    , textureHandle_(nullptr)
    , textureWidth_(1280.0f)
    , textureHeight_(720.0f)
    , currentWidth_(1280.0f)
    , currentHeight_(720.0f)
    , fixedAspectRatio_(true) {
}

void SceneViewWindow::Draw() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(GetWindowName(), &isVisible_);
    
    // ウィンドウのコンテンツ領域のサイズを取得
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    
    if (textureHandle_) {
        // アスペクト比を計算
        float aspectRatio = textureWidth_ / textureHeight_;
        float displayWidth = windowSize.x;
        float displayHeight = windowSize.y;
        
        if (fixedAspectRatio_) {
            // アスペクト比を維持
            if (displayWidth / displayHeight > aspectRatio) {
                displayWidth = displayHeight * aspectRatio;
            } else {
                displayHeight = displayWidth / aspectRatio;
            }
        }
        
        currentWidth_ = displayWidth;
        currentHeight_ = displayHeight;
        
        // 中央揃え
        float offsetX = (windowSize.x - displayWidth) * 0.5f;
        float offsetY = (windowSize.y - displayHeight) * 0.5f;
        
        if (offsetX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        if (offsetY > 0) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
        
        // テクスチャを描画
        ImGui::Image(textureHandle_, ImVec2(displayWidth, displayHeight));
        
    } else {
        // テクスチャが設定されていない場合
        ImGui::SetCursorPos(ImVec2(windowSize.x * 0.5f - 50, windowSize.y * 0.5f - 10));
        ImGui::TextDisabled("No texture assigned");
    }
    
    ImGui::End();
    ImGui::PopStyleVar();
}

void SceneViewWindow::SetTexture(void* srv_gpu_handle) {
    textureHandle_ = srv_gpu_handle;
}

// GameViewWindow の実装
GameViewWindow::GameViewWindow()
    : isVisible_(true)
    , textureHandle_(nullptr)
    , textureWidth_(1280.0f)
    , textureHeight_(720.0f)
    , fixedAspectRatio_(true)
    , showControls_(true)
    , showStats_(true)
    , currentFPS_(60.0f) {
}

void GameViewWindow::Draw() {
    ImGui::Begin(GetWindowName(), &isVisible_);
    
    // ツールバー
    if (showControls_) {
        if (ImGui::Button("Play")) {
            // 再生処理（外部から制御）
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            // 一時停止処理
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            // 停止処理
        }
        
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        
        ImGui::Checkbox("Fixed Aspect", &fixedAspectRatio_);
        
        if (showStats_) {
            ImGui::SameLine();
            ImGui::Text("FPS: %.1f", currentFPS_);
        }
        
        ImGui::Separator();
    }
    
    // ゲーム画面の描画領域
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    
    if (textureHandle_) {
        // アスペクト比を計算
        float aspectRatio = textureWidth_ / textureHeight_;
        float displayWidth = windowSize.x;
        float displayHeight = windowSize.y;
        
        if (fixedAspectRatio_) {
            // アスペクト比を維持
            if (displayWidth / displayHeight > aspectRatio) {
                displayWidth = displayHeight * aspectRatio;
            } else {
                displayHeight = displayWidth / aspectRatio;
            }
        }
        
        // 中央揃え
        float offsetX = (windowSize.x - displayWidth) * 0.5f;
        float offsetY = (windowSize.y - displayHeight) * 0.5f;
        
        if (offsetX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        if (offsetY > 0) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
        
        // テクスチャを描画（黒枠付き）
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        
        ImGui::Image(textureHandle_, ImVec2(displayWidth, displayHeight));
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        
        // マウスホバー時の情報表示
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Resolution: %.0fx%.0f", textureWidth_, textureHeight_);
            ImGui::Text("Display: %.0fx%.0f", displayWidth, displayHeight);
            ImGui::EndTooltip();
        }
        
    } else {
        // テクスチャが設定されていない場合
        ImGui::SetCursorPos(ImVec2(windowSize.x * 0.5f - 100, windowSize.y * 0.5f - 10));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("Press Play to start the game");
        ImGui::PopStyleColor();
    }
    
    ImGui::End();
}

void GameViewWindow::SetTexture(void* srv_gpu_handle) {
    textureHandle_ = srv_gpu_handle;
}

#endif // USE_IMGUI
