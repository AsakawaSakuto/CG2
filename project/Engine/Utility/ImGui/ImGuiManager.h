#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <string>

#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

class IImGuiWindow;

/// <summary>
/// Unityライクなデバッグウィンドウシステムのマネージャー
/// </summary>
class ImGuiManager {
public:
    static ImGuiManager* GetInstance();

    /// <summary>
    /// ウィンドウを登録
    /// </summary>
    void RegisterWindow(std::shared_ptr<IImGuiWindow> window);

    /// <summary>
    /// すべてのウィンドウを描画
    /// </summary>
    void DrawAllWindows();

    /// <summary>
    /// ウィンドウの表示/非表示を切り替え
    /// </summary>
    void ToggleWindow(const std::string& windowName);

    /// <summary>
    /// メインメニューバーを描画
    /// </summary>
    void DrawMainMenuBar();

    /// <summary>
    /// Unity風のレイアウトを設定
    /// </summary>
    void SetupUnityStyleLayout();

    /// <summary>
    /// レイアウトをリセット
    /// </summary>
    void ResetLayout();

    /// <summary>
    /// ドッキングを有効化
    /// </summary>
    void EnableDocking(bool enable) { isDockingEnabled_ = enable; }

private:
    ImGuiManager() : isDockingEnabled_(true), isLayoutInitialized_(false) {}
    ~ImGuiManager() = default;
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;

    std::vector<std::shared_ptr<IImGuiWindow>> windows_;
    bool isDockingEnabled_;
    bool isLayoutInitialized_;
};

/// <summary>
/// ImGuiウィンドウの基底インターフェース
/// </summary>
class IImGuiWindow {
public:
    virtual ~IImGuiWindow() = default;

    /// <summary>
    /// ウィンドウを描画
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// ウィンドウ名を取得
    /// </summary>
    virtual const char* GetWindowName() const = 0;

    /// <summary>
    /// ウィンドウの表示状態を取得
    /// </summary>
    virtual bool IsVisible() const = 0;

    /// <summary>
    /// ウィンドウの表示状態を設定
    /// </summary>
    virtual void SetVisible(bool visible) = 0;
};

#endif // USE_IMGUI
