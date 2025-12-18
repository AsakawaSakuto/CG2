#pragma once

#ifdef USE_IMGUI
#include "ImGuiManager.h"
#include "InspectorWindow.h"
#include "ConsoleWindow.h"
#include "HierarchyWindow.h"
#include "SceneViewWindow.h"
#include "StatsWindow.h"

/// <summary>
/// ImGuiデバッグシステムの使用例
/// </summary>
class ImGuiDebugSystem {
public:
    static ImGuiDebugSystem* GetInstance();

    /// <summary>
    /// 初期化（ゲーム起動時に1回呼ぶ）
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新（毎フレーム呼ぶ）
    /// </summary>
    void Update();

    /// <summary>
    /// 各ウィンドウへのアクセサ
    /// </summary>
    std::shared_ptr<InspectorWindow> GetInspector() { return inspector_; }
    std::shared_ptr<ConsoleWindow> GetConsole() { return console_; }
    std::shared_ptr<HierarchyWindow> GetHierarchy() { return hierarchy_; }
    std::shared_ptr<GameViewWindow> GetGameView() { return gameView_; }
    std::shared_ptr<SceneViewWindow> GetSceneView() { return sceneView_; }
    std::shared_ptr<StatsWindow> GetStats() { return stats_; }

private:
    ImGuiDebugSystem() = default;
    ~ImGuiDebugSystem() = default;
    ImGuiDebugSystem(const ImGuiDebugSystem&) = delete;
    ImGuiDebugSystem& operator=(const ImGuiDebugSystem&) = delete;

    std::shared_ptr<InspectorWindow> inspector_;
    std::shared_ptr<ConsoleWindow> console_;
    std::shared_ptr<HierarchyWindow> hierarchy_;
    std::shared_ptr<GameViewWindow> gameView_;
    std::shared_ptr<SceneViewWindow> sceneView_;
    std::shared_ptr<StatsWindow> stats_;
    bool initialized_ = false;
};

#endif // USE_IMGUI
