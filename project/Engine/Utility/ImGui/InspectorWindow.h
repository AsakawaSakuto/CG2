#pragma once

#ifdef USE_IMGUI
#include "ImGuiManager.h"
#include "ImGuiHelper.h"
#include <functional>
#include <vector>
#include <string>

/// <summary>
/// Unityのインスペクター風のプロパティウィンドウ
/// </summary>
class InspectorWindow : public IImGuiWindow {
public:
    InspectorWindow();
    ~InspectorWindow() override = default;

    void Draw() override;
    const char* GetWindowName() const override { return "Inspector"; }
    bool IsVisible() const override { return isVisible_; }
    void SetVisible(bool visible) override { isVisible_ = visible; }

    /// <summary>
    /// 選択中のオブジェクト名を設定
    /// </summary>
    void SetSelectedObjectName(const std::string& name) { selectedObjectName_ = name; }

    /// <summary>
    /// インスペクター描画用のカスタム関数を登録
    /// </summary>
    void SetDrawFunction(std::function<void()> func) { drawFunction_ = func; }

    /// <summary>
    /// インスペクターをクリア
    /// </summary>
    void Clear() {
        selectedObjectName_ = "None";
        drawFunction_ = nullptr;
    }

private:
    bool isVisible_;
    std::string selectedObjectName_;
    std::function<void()> drawFunction_;
};

#endif // USE_IMGUI
