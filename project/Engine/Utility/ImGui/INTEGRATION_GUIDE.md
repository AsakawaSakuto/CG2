# ImGui Unity-Style Debug System - 統合ガイド

## ?? 新機能：Unity風レイアウト

このシステムは自動的にUnity風のドッキングレイアウトを構築し、実行画面を美しく表示します。

## クイックスタート

### 1. 最も簡単な使い方

```cpp
// ゲームの初期化部分
#include "Engine/Utility/ImGui/ImGuiDebugSystem.h"

void GameInitialize() {
    // ... 他の初期化処理 ...
    
    #ifdef USE_IMGUI
    ImGuiDebugSystem::GetInstance()->Initialize();
    #endif
}

// ゲームのメインループ
void GameUpdate() {
    // ... ゲームロジック ...
    
    #ifdef USE_IMGUI
    // パフォーマンス統計を更新
    auto stats = ImGuiDebugSystem::GetInstance()->GetStats();
    stats->UpdateFPS(1.0f / deltaTime);
    stats->UpdateFrameTime(deltaTime * 1000.0f);
    
    // ImGui描画
    ImGuiDebugSystem::GetInstance()->Update();
    #endif
}
```

### 2. 実行画面をGameViewに表示する

```cpp
#include "Engine/Utility/ImGui/ImGuiDebugSystem.h"

class MyGame {
    ID3D12DescriptorHeap* srvHeap_;
    D3D12_GPU_DESCRIPTOR_HANDLE renderTextureSRV_;
    
    void Render() {
        // レンダーテクスチャに描画
        RenderToTexture();
        
        #ifdef USE_IMGUI
        // GameViewウィンドウに表示
        auto gameView = ImGuiDebugSystem::GetInstance()->GetGameView();
        gameView->SetTexture((ImTextureID)renderTextureSRV_.ptr);
        gameView->SetTextureSize(1280.0f, 720.0f);
        gameView->SetFixedAspectRatio(true);
        gameView->SetFPS(currentFPS);
        #endif
    }
};
```

## ?? パフォーマンス統計の表示

```cpp
void UpdateDebugInfo() {
    #ifdef USE_IMGUI
    auto stats = ImGuiDebugSystem::GetInstance()->GetStats();
    
    // 基本統計
    stats->UpdateFPS(currentFPS);
    stats->UpdateFrameTime(frameTimeMs);
    
    // レンダリング統計
    stats->SetDrawCalls(renderer->GetDrawCallCount());
    stats->SetTriangleCount(renderer->GetTriangleCount());
    stats->SetMemoryUsage(GetMemoryUsageMB());
    
    // ゲーム固有の統計
    stats->AddCustomStat("Enemies", std::to_string(enemyManager->GetCount()));
    stats->AddCustomStat("Bullets", std::to_string(bulletManager->GetCount()));
    stats->AddCustomStat("Score", std::to_string(player->GetScore()));
    #endif
}
```

## ?? ヒエラルキーにオブジェクトを追加

```cpp
void SetupGameHierarchy() {
    #ifdef USE_IMGUI
    auto hierarchy = ImGuiDebugSystem::GetInstance()->GetHierarchy();
    
    // プレイヤーオブジェクト
    auto player = std::make_shared<HierarchyWindow::GameObject>("Player");
    player->userData = playerObject; // 実際のゲームオブジェクトへのポインタ
    
    // 武器を子として追加
    auto weapon = std::make_shared<HierarchyWindow::GameObject>("Weapon");
    weapon->depth = 1;
    player->children.push_back(weapon);
    
    hierarchy->AddRootObject(player);
    
    // エネミー親オブジェクト
    auto enemies = std::make_shared<HierarchyWindow::GameObject>("Enemies");
    for (int i = 0; i < enemyCount; i++) {
        auto enemy = std::make_shared<HierarchyWindow::GameObject>(
            "Enemy_" + std::to_string(i));
        enemy->depth = 1;
        enemies->children.push_back(enemy);
    }
    hierarchy->AddRootObject(enemies);
    #endif
}
```

## ?? インスペクターでオブジェクトを表示

```cpp
// プレイヤーが選択されたときの処理
void OnPlayerSelected(Player* player) {
    #ifdef USE_IMGUI
    auto inspector = ImGuiDebugSystem::GetInstance()->GetInspector();
    inspector->SetSelectedObjectName("Player");
    
    inspector->SetDrawFunction([player]() {
        // Transform
        ImGuiHelper::DrawTransformControls(
            player->position,
            player->rotation,
            player->scale
        );
        
        // プレイヤーステータス
        if (ImGuiHelper::BeginPropertySection("Player Status", true)) {
            ImGui::DragFloat("HP", &player->hp, 1.0f, 0.0f, player->maxHp);
            ImGui::DragFloat("Speed", &player->speed, 0.1f, 0.0f, 20.0f);
            ImGui::Checkbox("Invincible", &player->isInvincible);
            
            // プログレスバー
            ImGuiHelper::ProgressBar(player->hp / player->maxHp, "HP");
            
            ImGuiHelper::EndPropertySection();
        }
        
        // デバッグアクション
        if (ImGuiHelper::BeginPropertySection("Debug Actions", false)) {
            if (ImGui::Button("Full Heal")) {
                player->hp = player->maxHp;
                ImGuiLog::Success("Player healed!");
            }
            ImGui::SameLine();
            if (ImGui::Button("God Mode")) {
                player->isInvincible = !player->isInvincible;
            }
            
            if (ImGuiHelper::ConfirmButton("Reset Position", "位置をリセットしますか？")) {
                player->position = {0.0f, 0.0f, 0.0f};
                ImGuiLog::Info("Position reset");
            }
            
            ImGuiHelper::EndPropertySection();
        }
    });
    #endif
}
```

## ?? ログの使用

```cpp
#include "Engine/Utility/ImGui/ConsoleWindow.h"

void GameEvents() {
    // ゲームイベントでログを出力
    ImGuiLog::Info("ゲーム開始");
    ImGuiLog::Success("ミッションクリア！");
    ImGuiLog::Warning("HPが残り20%です");
    ImGuiLog::Error("ゲームオーバー");
}

// カスタム関数でも使用可能
void LoadResource(const std::string& path) {
    try {
        // リソース読み込み
        ImGuiLog::Success("Loaded: " + path);
    } catch (...) {
        ImGuiLog::Error("Failed to load: " + path);
    }
}
```

## ?? レイアウトのカスタマイズ

### プログラムからレイアウトを変更

```cpp
// Unity風レイアウトをリセット
ImGuiManager::GetInstance()->ResetLayout();

// ドッキングを無効化（シンプルモード）
ImGuiManager::GetInstance()->EnableDocking(false);

// 再度ドッキングを有効化
ImGuiManager::GetInstance()->EnableDocking(true);
ImGuiManager::GetInstance()->ResetLayout();
```

### ユーザー操作
- **メニューバー > Window > Reset Layout**: レイアウトをリセット
- **メニューバー > Layout > Unity Style**: Unity風レイアウト
- **メニューバー > Layout > Simple**: ドッキングを無効化

## ??? レイアウト構成

デフォルトのUnity風レイアウト：

```
┌──────────────────────────────────────────────────────────┐
│ Menu Bar [Window] [Layout]                               │
├─────────────┬────────────────────────┬────────────────────┤
│             │                        │                    │
│  Hierarchy  │     Game View          │    Inspector       │
│             │                        │                    │
│   (20%)     │  - Play/Pause controls │      (20%)         │
│             │  - FPS display         │                    │
│  - Player   │  - Aspect ratio lock   │  - Transform       │
│  - Enemies  │  - Game render texture │  - Properties      │
│  - Bullets  │                        │  - Debug actions   │
│             │                        │                    │
├─────────────┴────────────────────────┴────────────────────┤
│                                                            │
│                     Console                                │
│                      (30%)                                 │
│  - Info/Warning/Error/Success logs                         │
│  - Timestamp                                               │
│  - Search filter                                           │
│                                                            │
├────────────────────────────────────────────────────────────┤
│                     Stats (Optional)                       │
│  - FPS graph | Frame time | Draw calls                     │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

## ?? 完全な実装例

```cpp
// Game.h
#pragma once
#ifdef USE_IMGUI
#include "Engine/Utility/ImGui/ImGuiDebugSystem.h"
#endif

class Game {
public:
    void Initialize() {
        // ゲーム初期化
        InitializeGame();
        
        #ifdef USE_IMGUI
        // ImGuiシステム初期化
        ImGuiDebugSystem::GetInstance()->Initialize();
        SetupDebugHierarchy();
        ImGuiLog::Success("Game initialized!");
        #endif
    }
    
    void Update(float deltaTime) {
        // ゲームロジック更新
        UpdateGame(deltaTime);
        
        #ifdef USE_IMGUI
        UpdateDebugUI(deltaTime);
        #endif
    }
    
    void Render() {
        // ゲーム描画
        RenderGame();
        
        #ifdef USE_IMGUI
        RenderDebugUI();
        #endif
    }
    
private:
    void UpdateDebugUI(float deltaTime) {
        auto stats = ImGuiDebugSystem::GetInstance()->GetStats();
        
        // FPS更新
        float fps = 1.0f / deltaTime;
        stats->UpdateFPS(fps);
        stats->UpdateFrameTime(deltaTime * 1000.0f);
        
        // レンダリング統計
        stats->SetDrawCalls(renderer_->GetDrawCallCount());
        stats->SetTriangleCount(renderer_->GetTriangleCount());
        
        // ゲーム統計
        stats->AddCustomStat("Player HP", std::to_string((int)player_->GetHP()));
        stats->AddCustomStat("Enemies", std::to_string(enemyManager_->GetCount()));
        stats->AddCustomStat("Score", std::to_string(score_));
        
        // ImGui更新
        ImGuiDebugSystem::GetInstance()->Update();
    }
    
    void RenderDebugUI() {
        // ゲーム画面をGameViewに表示
        auto gameView = ImGuiDebugSystem::GetInstance()->GetGameView();
        gameView->SetTexture((ImTextureID)renderTextureSRV_.ptr);
        gameView->SetTextureSize(1280.0f, 720.0f);
        gameView->SetFixedAspectRatio(true);
    }
    
    void SetupDebugHierarchy() {
        auto hierarchy = ImGuiDebugSystem::GetInstance()->GetHierarchy();
        
        // ヒエラルキー構築
        auto player = std::make_shared<HierarchyWindow::GameObject>("Player");
        hierarchy->AddRootObject(player);
        
        auto enemies = std::make_shared<HierarchyWindow::GameObject>("Enemies");
        hierarchy->AddRootObject(enemies);
        
        // 選択コールバック（既に設定済み）
        hierarchy->SetSelectionCallback([this](HierarchyWindow::GameObject* obj) {
            OnObjectSelected(obj);
        });
    }
    
    void OnObjectSelected(HierarchyWindow::GameObject* obj) {
        if (obj->name == "Player") {
            ShowPlayerInspector();
        }
    }
    
    void ShowPlayerInspector() {
        auto inspector = ImGuiDebugSystem::GetInstance()->GetInspector();
        inspector->SetSelectedObjectName("Player");
        
        inspector->SetDrawFunction([this]() {
            ImGuiHelper::DrawTransformControls(
                player_->position,
                player_->rotation,
                player_->scale
            );
            
            if (ImGuiHelper::BeginPropertySection("Player Stats")) {
                ImGui::DragFloat("HP", &player_->hp, 1.0f, 0.0f, 100.0f);
                ImGui::DragFloat("Speed", &player_->speed, 0.1f);
                ImGuiHelper::EndPropertySection();
            }
        });
    }
};
```

## ?? ヒント

### ウィンドウの操作
1. **タイトルバーをドラッグ**: ウィンドウを移動
2. **境界をドラッグ**: サイズ変更
3. **他のウィンドウにドロップ**: ドッキング
4. **×ボタン**: ウィンドウを非表示（メニューから再表示可能）

### パフォーマンス最適化
- ウィンドウを非表示にすると描画負荷が減少
- リリースビルドでは`USE_IMGUI`を定義しない
- 統計情報の更新頻度を調整可能

### デバッグ時のワークフロー
1. **Hierarchy**でオブジェクトを選択
2. **Inspector**でプロパティを編集
3. **Game View**で結果を確認
4. **Console**でログを確認
5. **Stats**でパフォーマンスを監視

## ?? トラブルシューティング

### Q: ドッキングレイアウトが表示されない
A: ImGuiのドッキング機能が有効になっているか確認してください。`imgui_internal.h`がインクルードされている必要があります。

### Q: Game Viewにテクスチャが表示されない
A: `SetTexture()`に渡すハンドルが`ImTextureID`型（GPU Descriptor Handleのポインタ値）であることを確認してください。

### Q: レイアウトが崩れた
A: メニューバーの「Window > Reset Layout」でリセットできます。

### Q: FPSグラフが表示されない
A: `UpdateFPS()`と`UpdateFrameTime()`を毎フレーム呼び出していることを確認してください。

## ?? まとめ

このシステムで実現できること：

? **Unity風の直感的なレイアウト**
  - 自動ドッキング
  - ドラッグ＆ドロップで再配置
  - メニューからリセット可能

? **ゲーム画面の統合表示**
  - Game View/Scene Viewで描画結果を確認
  - アスペクト比固定
  - 再生コントロール

? **リアルタイム統計**
  - FPS/フレーム時間のグラフ
  - 描画コール数・三角形数
  - カスタム統計情報

? **完全なデバッグ環境**
  - ヒエラルキーでオブジェクト管理
  - インスペクターでプロパティ編集
  - コンソールでログ確認

? **プロダクションレディ**
  - USE_IMGUIマクロで完全に除去可能
  - リリースビルドへの影響なし
  - パフォーマンス最適化済み

楽しい開発ライフを！???
