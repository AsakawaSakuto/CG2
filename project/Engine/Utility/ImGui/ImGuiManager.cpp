#include "ImGuiManager.h"

#ifdef USE_IMGUI
#include "imgui_internal.h" // ドッキングAPIのために必要

ImGuiManager* ImGuiManager::GetInstance() {
    static ImGuiManager instance;
    return &instance;
}

void ImGuiManager::RegisterWindow(std::shared_ptr<IImGuiWindow> window) {
    if (window) {
        windows_.push_back(window);
    }
}

void ImGuiManager::DrawAllWindows() {
    // ドッキングスペースを作成
    if (isDockingEnabled_) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
        ImGui::PopStyleVar(3);
        
        // メニューバーを描画
        DrawMainMenuBar();
        
        // ドッキングスペース
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        
        // 初回のみレイアウトを設定
        if (!isLayoutInitialized_) {
            SetupUnityStyleLayout();
            isLayoutInitialized_ = true;
        }
        
        ImGui::End();
    } else {
        DrawMainMenuBar();
    }
    
    // 各ウィンドウを描画
    for (auto& window : windows_) {
        if (window && window->IsVisible()) {
            window->Draw();
        }
    }
}

void ImGuiManager::ToggleWindow(const std::string& windowName) {
    for (auto& window : windows_) {
        if (window && window->GetWindowName() == windowName) {
            window->SetVisible(!window->IsVisible());
            break;
        }
    }
}

void ImGuiManager::DrawMainMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Window")) {
            for (auto& window : windows_) {
                if (window) {
                    bool visible = window->IsVisible();
                    if (ImGui::MenuItem(window->GetWindowName(), nullptr, visible)) {
                        window->SetVisible(!visible);
                    }
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Layout")) {
                ResetLayout();
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Layout")) {
            if (ImGui::MenuItem("Unity Style")) {
                ResetLayout();
            }
            if (ImGui::MenuItem("Simple")) {
                isLayoutInitialized_ = false;
                isDockingEnabled_ = false;
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
}

void ImGuiManager::SetupUnityStyleLayout() {
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);
    
    // レイアウト分割
    // 左側：ヒエラルキー（20%）
    // 中央：ゲーム画面/シーンビュー（60%）
    // 右側：インスペクター（20%）
    // 下部：コンソール（30%の高さ）
    
    ImGuiID dock_main_id = dockspace_id;
    ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
    ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
    ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
    
    // 右側をさらに上下に分割（Inspector上、その他下）
    ImGuiID dock_right_top = dock_right_id;
    ImGuiID dock_right_bottom = ImGui::DockBuilderSplitNode(dock_right_top, ImGuiDir_Down, 0.5f, nullptr, &dock_right_top);
    
    // ウィンドウを各ドックに配置
    // 左側
    ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
    
    // 右上
    ImGui::DockBuilderDockWindow("Inspector", dock_right_top);
    
    // 右下（デバッグウィンドウなど）
    ImGui::DockBuilderDockWindow("testOBB", dock_right_bottom);
    ImGui::DockBuilderDockWindow("testAABB", dock_right_bottom);
    ImGui::DockBuilderDockWindow("testSphere", dock_right_bottom);
    ImGui::DockBuilderDockWindow("testOvalSphere", dock_right_bottom);
    ImGui::DockBuilderDockWindow("testPlane", dock_right_bottom);
    ImGui::DockBuilderDockWindow("Set Number", dock_right_bottom);
    ImGui::DockBuilderDockWindow("bitmapFont", dock_right_bottom);
    
    // 下部
    ImGui::DockBuilderDockWindow("Console", dock_bottom_id);
    
    // 左下（その他のデバッグウィンドウ）
    ImGuiID dock_left_bottom = ImGui::DockBuilderSplitNode(dock_left_id, ImGuiDir_Down, 0.4f, nullptr, &dock_left_id);
    ImGui::DockBuilderDockWindow("デバッグカメラ操作", dock_left_bottom);
    ImGui::DockBuilderDockWindow("Post Effect Settings", dock_left_bottom);
    ImGui::DockBuilderDockWindow("testParticle", dock_left_bottom);
    ImGui::DockBuilderDockWindow("Stats", dock_left_bottom);
    
    // 中央にはゲーム画面やScene View（dock_main_id）
    // Game ViewとScene Viewをタブで表示
    ImGui::DockBuilderDockWindow("Game View", dock_main_id);
    ImGui::DockBuilderDockWindow("Scene View", dock_main_id);
    
    ImGui::DockBuilderFinish(dockspace_id);
}

void ImGuiManager::ResetLayout() {
    isLayoutInitialized_ = false;
    isDockingEnabled_ = true;
}

#endif // USE_IMGUI
