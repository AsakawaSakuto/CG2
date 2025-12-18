#pragma once

#ifdef USE_IMGUI
#include "ImGuiManager.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>

/// <summary>
/// Unityのヒエラルキー風のシーンオブジェクト一覧ウィンドウ
/// </summary>
class HierarchyWindow : public IImGuiWindow {
public:
    /// <summary>
    /// ヒエラルキーに表示するオブジェクトの情報
    /// </summary>
    struct GameObject {
        std::string name;
        bool isActive;
        int depth; // 階層の深さ（インデント用）
        void* userData; // ユーザーデータへのポインタ（任意）
        std::vector<std::shared_ptr<GameObject>> children;
        
        GameObject(const std::string& n = "GameObject", bool active = true, int d = 0)
            : name(n), isActive(active), depth(d), userData(nullptr) {}
    };

    HierarchyWindow();
    ~HierarchyWindow() override = default;

    void Draw() override;
    const char* GetWindowName() const override { return "Hierarchy"; }
    bool IsVisible() const override { return isVisible_; }
    void SetVisible(bool visible) override { isVisible_ = visible; }

    /// <summary>
    /// ルートオブジェクトを追加
    /// </summary>
    void AddRootObject(std::shared_ptr<GameObject> obj);

    /// <summary>
    /// すべてのオブジェクトをクリア
    /// </summary>
    void Clear();

    /// <summary>
    /// オブジェクト選択時のコールバック設定
    /// </summary>
    void SetSelectionCallback(std::function<void(GameObject*)> callback) {
        selectionCallback_ = callback;
    }

    /// <summary>
    /// 現在選択中のオブジェクトを取得
    /// </summary>
    GameObject* GetSelectedObject() { return selectedObject_; }

private:
    bool isVisible_;
    std::vector<std::shared_ptr<GameObject>> rootObjects_;
    GameObject* selectedObject_;
    std::function<void(GameObject*)> selectionCallback_;
    char searchBuffer_[256];

    void DrawGameObjectNode(GameObject* obj);
    bool MatchesSearch(const GameObject* obj);
};

#endif // USE_IMGUI
