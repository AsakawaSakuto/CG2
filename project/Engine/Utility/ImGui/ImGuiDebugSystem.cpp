#include "ImGuiDebugSystem.h"

#ifdef USE_IMGUI

ImGuiDebugSystem* ImGuiDebugSystem::GetInstance() {
    static ImGuiDebugSystem instance;
    return &instance;
}

void ImGuiDebugSystem::Initialize() {
    if (initialized_) {
        return;
    }

    auto* manager = ImGuiManager::GetInstance();

    // ドッキングを有効化
    manager->EnableDocking(true);

    // ウィンドウを作成
    inspector_ = std::make_shared<InspectorWindow>();
    console_ = std::make_shared<ConsoleWindow>();
    hierarchy_ = std::make_shared<HierarchyWindow>();
    gameView_ = std::make_shared<GameViewWindow>();
    sceneView_ = std::make_shared<SceneViewWindow>();
    stats_ = std::make_shared<StatsWindow>();

    // マネージャーに登録
    manager->RegisterWindow(hierarchy_);
    manager->RegisterWindow(inspector_);
    manager->RegisterWindow(console_);
    manager->RegisterWindow(gameView_);
    manager->RegisterWindow(sceneView_);
    manager->RegisterWindow(stats_);

    // ヒエラルキーの選択コールバック設定
    hierarchy_->SetSelectionCallback([this](HierarchyWindow::GameObject* obj) {
        if (obj) {
            // 選択されたオブジェクトの情報をインスペクターに表示
            inspector_->SetSelectedObjectName(obj->name);
            
            // カスタム描画関数を設定
            inspector_->SetDrawFunction([obj]() {
                // オブジェクト情報の表示
                ImGuiHelper::ReadOnlyText("Name", obj->name.c_str());
                ImGuiHelper::ReadOnlyInt("Depth", obj->depth);
                
                ImGuiHelper::StyledSeparator();
                
                // アクティブ状態
                ImGui::Checkbox("Active", &obj->isActive);
                
                // 子オブジェクトの数
                ImGuiHelper::ReadOnlyInt("Children Count", static_cast<int>(obj->children.size()));
            });
            
            // ログに記録
            ImGuiLog::Info("Selected: " + obj->name);
        }
    });

    // 初期ログ
    ImGuiLog::Success("ImGui Debug System initialized");
    ImGuiLog::Info("Unity-style layout enabled");
    ImGuiLog::Info("Use Window menu to toggle panels");

    initialized_ = true;
}

void ImGuiDebugSystem::Update() {
    if (!initialized_) {
        Initialize();
    }

    // すべてのウィンドウを描画
    ImGuiManager::GetInstance()->DrawAllWindows();
}

#endif // USE_IMGUI
