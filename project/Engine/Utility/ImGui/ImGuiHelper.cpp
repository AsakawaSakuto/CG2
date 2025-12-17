#include "ImGuiHelper.h"

#ifdef USE_IMGUI
#include <cmath>

namespace ImGuiHelper {

    bool DrawVector3Control(const char* label, Vector3& v, const Vector3& resetValue, float speed) {
        bool changed = false;
        ImGui::PushID(label);
        
        ImGui::Text("%s", label);
        ImGui::SameLine();
        
        // X軸（赤）
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
        if (ImGui::Button("X", ImVec2(20, 20))) {
            v.x = resetValue.x;
            changed = true;
        }
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        if (ImGui::DragFloat("##X", &v.x, speed)) {
            changed = true;
        }
        
        ImGui::SameLine();
        
        // Y軸（緑）
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        if (ImGui::Button("Y", ImVec2(20, 20))) {
            v.y = resetValue.y;
            changed = true;
        }
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        if (ImGui::DragFloat("##Y", &v.y, speed)) {
            changed = true;
        }
        
        ImGui::SameLine();
        
        // Z軸（青）
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
        if (ImGui::Button("Z", ImVec2(20, 20))) {
            v.z = resetValue.z;
            changed = true;
        }
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        if (ImGui::DragFloat("##Z", &v.z, speed)) {
            changed = true;
        }
        
        ImGui::PopID();
        return changed;
    }

    bool BeginPropertySection(const char* label, bool defaultOpen) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
        if (!defaultOpen) {
            flags &= ~ImGuiTreeNodeFlags_DefaultOpen;
        }
        return ImGui::CollapsingHeader(label, flags);
    }

    void EndPropertySection() {
        // 必要に応じて後処理
    }

    void ReadOnlyText(const char* label, const char* text) {
        ImGui::Text("%s: %s", label, text);
    }

    void ReadOnlyFloat(const char* label, float value) {
        ImGui::Text("%s: %.3f", label, value);
    }

    void ReadOnlyInt(const char* label, int value) {
        ImGui::Text("%s: %d", label, value);
    }

    bool ConfirmButton(const char* label, const char* confirmText) {
        static bool showConfirm = false;
        static const char* currentLabel = nullptr;
        
        bool result = false;
        
        if (ImGui::Button(label)) {
            showConfirm = true;
            currentLabel = label;
        }
        
        if (showConfirm && currentLabel == label) {
            ImGui::OpenPopup("Confirm");
        }
        
        if (ImGui::BeginPopupModal("Confirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", confirmText);
            ImGui::Separator();
            
            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                result = true;
                showConfirm = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0))) {
                showConfirm = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        
        return result;
    }

    void ColoredText(const char* text, LogLevel level) {
        ImVec4 color;
        switch (level) {
            case LogLevel::Info:
                color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 白
                break;
            case LogLevel::Warning:
                color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // 黄色
                break;
            case LogLevel::Error:
                color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 赤
                break;
            case LogLevel::Success:
                color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // 緑
                break;
        }
        ImGui::TextColored(color, "%s", text);
    }

    void DrawTransformControls(Vector3& position, Vector3& rotation, Vector3& scale) {
        if (BeginPropertySection("Transform")) {
            DrawVector3Control("Position", position, {0.0f, 0.0f, 0.0f}, 0.1f);
            DrawVector3Control("Rotation", rotation, {0.0f, 0.0f, 0.0f}, 0.5f);
            DrawVector3Control("Scale", scale, {1.0f, 1.0f, 1.0f}, 0.01f);
            EndPropertySection();
        }
    }

    void StyledSeparator() {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }

    void HelpMarker(const char* desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    bool SearchBar(char* buffer, size_t bufferSize, const char* hint) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
        bool result = ImGui::InputTextWithHint("##search", hint, buffer, bufferSize);
        ImGui::PopStyleVar();
        return result;
    }

    void ProgressBar(float fraction, const char* overlay) {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
        ImGui::ProgressBar(fraction, ImVec2(-1, 0), overlay);
        ImGui::PopStyleColor();
    }

} // namespace ImGuiHelper

#endif // USE_IMGUI
