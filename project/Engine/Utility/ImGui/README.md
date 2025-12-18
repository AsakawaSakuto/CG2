# ImGui Unity-style Debug System

Unityのようなゲームエンジン風のデバッグUIシステムです。

## 新機能：Unity風レイアウト

### ドッキングシステム
- 自動的にUnity風のレイアウトを構築
- ウィンドウをドラッグ＆ドロップで自由に配置可能
- メニューバーから「Layout > Reset Layout」でレイアウトをリセット

### デフォルトレイアウト
```
┌──────────────────────────────────────────────────────┐
│ Menu Bar                                             │
├──────────┬──────────────────────────┬────────────────┤
│          │                          │                │
│ Hierarchy│    Game View / Scene     │   Inspector    │
│          │                          │                │
│  (20%)   │        (60%)             │     (20%)      │
│          │                          │                │
├──────────┴──────────────────────────┴────────────────┤
│                                                       │
│                    Console                            │
│                     (30%)                             │
│                                                       │
└───────────────────────────────────────────────────────┘
```

## 機能

### 1. ImGuiManager
ImGuiウィンドウの管理システム。メインメニューバーからウィンドウの表示/非表示を切り替えできます。

**新機能:**
- Unity風のドッキングレイアウト自動構築
- レイアウトのリセット機能
- ドラッグ＆ドロップでウィンドウを再配置

### 2. GameViewWindow（NEW!）
実行画面を表示する専用ウィンドウ
- アスペクト比固定/可変
- 再生/一時停止コントロール（予定）
- FPS表示
- レンダーテクスチャを表示可能

### 3. SceneViewWindow（NEW!）
シーンビューを表示するウィンドウ
- レンダーテクスチャを表示
- アスペクト比固定/可変
- 中央揃えで表示

### 4. StatsWindow（NEW!）
パフォーマンス統計情報ウィンドウ
- FPS履歴グラフ
- フレーム時間ヒストグラム
- 描画コール数
- 三角形数
- メモリ使用量
- カスタム統計情報

### 5. ImGuiHelper
汎用的なImGui拡張機能を提供します。
- Vector3編集ウィジェット（X/Y/Zボタン付き）
- 折りたたみ可能なプロパティセクション
- 読み取り専用フィールド
- カラーコード付きログテキスト
- Transform編集ウィジェット
- 検索バー
- プログレスバー

### 6. InspectorWindow
Unityのインスペクター風のプロパティ表示ウィンドウ。
選択されたオブジェクトのプロパティをカスタム描画できます。

### 7. ConsoleWindow
Unityのコンソール風のログ表示ウィンドウ。
- ログレベル別フィルタリング（Info/Warning/Error/Success）
- テキスト検索
- タイムスタンプ付き
- 自動スクロール
- 同じメッセージの繰り返し回数表示

### 8. HierarchyWindow
Unityのヒエラルキー風のシーンオブジェクト一覧ウィンドウ。
- ツリー構造でオブジェクトを表示
- オブジェクトの選択
- 右クリックメニュー
- 検索機能

## 使用例

### 基本的なセットアップ

```cpp
#include "Engine/Utility/ImGui/ImGuiDebugSystem.h"

// 初期化（ゲーム起動時に1回）
void Initialize() {
    ImGuiDebugSystem::GetInstance()->Initialize();
}

// 毎フレーム描画
void Update() {
    ImGuiDebugSystem::GetInstance()->Update();
}
```

### ゲーム画面をGameViewに表示

```cpp
#include "Engine/Utility/ImGui/ImGuiDebugSystem.h"

void RenderGame() {
    // レンダーテクスチャに描画...
    
    // GameViewウィンドウに設定
    auto gameView = ImGuiDebugSystem::GetInstance()->GetGameView();
    gameView->SetTexture(renderTextureSRV); // GPU Descriptor Handle
    gameView->SetTextureSize(1280.0f, 720.0f);
    gameView->SetFixedAspectRatio(true);
}
```

### パフォーマンス統計の更新

```cpp
void UpdateStats() {
    auto stats = ImGuiDebugSystem::GetInstance()->GetStats();
    
    // FPSを更新
    stats->UpdateFPS(currentFPS);
    stats->UpdateFrameTime(frameTimeMs);
    
    // レンダリング統計
    stats->SetDrawCalls(drawCallCount);
    stats->SetTriangleCount(triangleCount);
    stats->SetMemoryUsage(memoryUsageMB);
    
    // カスタム統計
    stats->AddCustomStat("Enemies", std::to_string(enemyCount));
    stats->AddCustomStat("Bullets", std::to_string(bulletCount));
}
```

### ログの使用

```cpp
#include "Engine/Utility/ImGui/ConsoleWindow.h"

void SomeFunction() {
    ImGuiLog::Info("ゲームを開始しました");
    ImGuiLog::Warning("警告メッセージ");
    ImGuiLog::Error("エラーが発生しました");
    ImGuiLog::Success("処理が成功しました");
}
```

### レイアウトのカスタマイズ

```cpp
// レイアウトをリセット
ImGuiManager::GetInstance()->ResetLayout();

// ドッキングを無効化（シンプルモード）
ImGuiManager::GetInstance()->EnableDocking(false);
```

## メニューバーの使い方

### Windowメニュー
- 各ウィンドウの表示/非表示を切り替え
- チェックマークで現在の表示状態を確認
- Reset Layoutでレイアウトをリセット

### Layoutメニュー
- **Unity Style**: Unity風のレイアウトに変更
- **Simple**: ドッキングを無効化してシンプルモードに

## ウィンドウの操作

### ドラッグ＆ドロップ
- ウィンドウのタイトルバーをドラッグして移動
- 他のウィンドウの端にドロップしてドッキング
- タブ形式でまとめることも可能

### リサイズ
- ウィンドウの境界をドラッグしてサイズ変更
- ダブルクリックで最適なサイズに

### 最小化/最大化
- タイトルバーの「-」ボタンで折りたたみ
- 「×」ボタンで非表示（メニューから再表示可能）

## 実践例：完全なゲームループ

```cpp
// Game.h
class Game {
public:
    void Initialize() {
        // ImGuiシステム初期化
        ImGuiDebugSystem::GetInstance()->Initialize();
        
        // ゲームオブジェクトをヒエラルキーに追加
        SetupHierarchy();
    }
    
    void Update() {
        // ゲームロジック更新
        UpdateGameLogic();
        
        // 統計情報更新
        UpdateDebugStats();
        
        // ImGui描画
        ImGuiDebugSystem::GetInstance()->Update();
    }
    
private:
    void SetupHierarchy() {
        auto hierarchy = ImGuiDebugSystem::GetInstance()->GetHierarchy();
        
        // プレイヤー
        auto player = std::make_shared<HierarchyWindow::GameObject>("Player");
        hierarchy->AddRootObject(player);
        
        // エネミー
        auto enemies = std::make_shared<HierarchyWindow::GameObject>("Enemies");
        for (int i = 0; i < 5; i++) {
            auto enemy = std::make_shared<HierarchyWindow::GameObject>(
                "Enemy_" + std::to_string(i));
            enemy->depth = 1;
            enemies->children.push_back(enemy);
        }
        hierarchy->AddRootObject(enemies);
    }
    
    void UpdateDebugStats() {
        auto stats = ImGuiDebugSystem::GetInstance()->GetStats();
        
        // FPS計算
        float fps = 1.0f / deltaTime;
        stats->UpdateFPS(fps);
        stats->UpdateFrameTime(deltaTime * 1000.0f);
        
        // ゲーム統計
        stats->AddCustomStat("Player HP", std::to_string(player->GetHP()));
        stats->AddCustomStat("Score", std::to_string(score));
    }
};
```

## トラブルシューティング

### Q: ウィンドウが表示されない
A: `USE_IMGUI`マクロが定義されていることを確認してください。

### Q: レイアウトが崩れた
A: メニューバーの「Window > Reset Layout」でリセットしてください。

### Q: GameViewにテクスチャが表示されない
A: `SetTexture()`でGPU Descriptor Handleが正しく設定されているか確認してください。

### Q: ドッキングが動作しない
A: ImGuiのドッキング機能が有効になっているか確認してください。

## パフォーマンスへの影響

- すべてのImGuiコードは`#ifdef USE_IMGUI`で保護されています
- リリースビルドでは`USE_IMGUI`を定義しないことで、コードが完全に除去されます
- デバッグビルドでもウィンドウを非表示にすれば描画負荷はほぼゼロです

## まとめ

このシステムにより、Unityのような直感的なデバッグ環境が実現できます：

? Unity風のドッキングレイアウト
? ゲーム画面とシーンビューの分離表示
? リアルタイムパフォーマンス統計
? メニューバーからウィンドウの表示/非表示を切り替え
? コンソールでカラー付きログを確認
? ヒエラルキーでオブジェクトを階層表示・選択
? インスペクターで選択したオブジェクトのプロパティを編集
? 汎用的なImGuiヘルパー関数で美しいUI
? すべてのコードはUSE_IMGUIマクロで保護され、リリースビルドには影響なし

楽しいデバッグライフを！
