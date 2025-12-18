#include "HierarchyWindow.h"
#include "ImGuiHelper.h"
#include <algorithm>

#ifdef USE_IMGUI

HierarchyWindow::HierarchyWindow()
    : isVisible_(true)
    , selectedObject_(nullptr)
    , selectionCallback_(nullptr) {
    memset(searchBuffer_, 0, sizeof(searchBuffer_));
}

void HierarchyWindow::Draw() {
    ImGui::Begin(GetWindowName(), &isVisible_);

    // ツールバー
    if (ImGui::Button("+ Create")) {
        ImGui::OpenPopup("CreateMenu");
    }
    
    if (ImGui::BeginPopup("CreateMenu")) {
        if (ImGui::MenuItem("Empty GameObject")) {
            auto newObj = std::make_shared<GameObject>("GameObject");
            AddRootObject(newObj);
        }
        if (ImGui::MenuItem("Camera")) {
            auto newObj = std::make_shared<GameObject>("Camera");
            AddRootObject(newObj);
        }
        if (ImGui::MenuItem("Light")) {
            auto newObj = std::make_shared<GameObject>("Light");
            AddRootObject(newObj);
        }
        ImGui::EndPopup();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Clear All")) {
        Clear();
    }
    
    ImGui::SameLine();
    ImGui::PushItemWidth(200);
    ImGuiHelper::SearchBar(searchBuffer_, sizeof(searchBuffer_), "Search objects...");
    ImGui::PopItemWidth();
    
    ImGui::Separator();

    // オブジェクトツリー
    ImGui::BeginChild("ObjectTree");
    
    if (rootObjects_.empty()) {
        ImGui::TextDisabled("No objects in scene");
    } else {
        for (auto& obj : rootObjects_) {
            if (obj) {
                DrawGameObjectNode(obj.get());
            }
        }
    }
    
    ImGui::EndChild();
    ImGui::End();
}

void HierarchyWindow::DrawGameObjectNode(GameObject* obj) {
    if (!obj || !MatchesSearch(obj)) {
        return;
    }
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    
    // 選択状態
    if (selectedObject_ == obj) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    
    // 子がいない場合は葉ノード
    if (obj->children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    
    // アクティブ状態の表示
    ImGui::PushStyleColor(ImGuiCol_Text, obj->isActive ? 
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : 
        ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    
    bool nodeOpen = ImGui::TreeNodeEx(obj, flags, "%s", obj->name.c_str());
    
    ImGui::PopStyleColor();
    
    // クリックで選択
    if (ImGui::IsItemClicked()) {
        selectedObject_ = obj;
        if (selectionCallback_) {
            selectionCallback_(obj);
        }
    }
    
    // 右クリックメニュー
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Rename")) {
            // リネーム機能（実装例）
        }
        if (ImGui::MenuItem("Duplicate")) {
            // 複製機能
        }
        if (ImGui::MenuItem("Delete")) {
            // 削除機能
        }
        ImGui::Separator();
        if (ImGui::MenuItem(obj->isActive ? "Deactivate" : "Activate")) {
            obj->isActive = !obj->isActive;
        }
        ImGui::EndPopup();
    }
    
    // 子ノードの描画
    if (nodeOpen && !obj->children.empty()) {
        for (auto& child : obj->children) {
            if (child) {
                DrawGameObjectNode(child.get());
            }
        }
        ImGui::TreePop();
    }
}

void HierarchyWindow::AddRootObject(std::shared_ptr<GameObject> obj) {
    if (obj) {
        rootObjects_.push_back(obj);
    }
}

void HierarchyWindow::Clear() {
    rootObjects_.clear();
    selectedObject_ = nullptr;
}

bool HierarchyWindow::MatchesSearch(const GameObject* obj) {
    if (!obj) return false;
    
    // 検索フィルター
    if (strlen(searchBuffer_) > 0) {
        return obj->name.find(searchBuffer_) != std::string::npos;
    }
    
    return true;
}

#endif // USE_IMGUI
