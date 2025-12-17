#include "InspectorWindow.h"

#ifdef USE_IMGUI

InspectorWindow::InspectorWindow()
    : isVisible_(true)
    , selectedObjectName_("None")
    , drawFunction_(nullptr) {
}

void InspectorWindow::Draw() {
    ImGui::Begin(GetWindowName(), &isVisible_);

    // オブジェクト名表示
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("Selected: %s", selectedObjectName_.c_str());
    ImGui::PopStyleColor();
    
    ImGuiHelper::StyledSeparator();

    // カスタム描画関数がある場合は実行
    if (drawFunction_) {
        drawFunction_();
    } else {
        ImGui::TextDisabled("No object selected");
        ImGui::Spacing();
        ImGui::TextWrapped("Select an object in the scene or hierarchy to view its properties.");
    }

    ImGui::End();
}

#endif // USE_IMGUI
