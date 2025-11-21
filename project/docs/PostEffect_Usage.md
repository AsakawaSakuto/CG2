# ポストエフェクトの使用方法

このドキュメントでは、新しく追加されたポストエフェクト機能の使い方を説明します。

## 利用可能なポストエフェクト

以下のポストエフェクトが実装されています：

1. **Offscreen_None** - エフェクトなし（通常描画）
2. **Offscreen_Grayscale** - グレースケール（白黒）
3. **Offscreen_Sepia** - セピア調（古い写真風）
4. **Offscreen_Vignette** - ビネット効果（周辺減光）
5. **Offscreen_Invert** - 色反転（ネガポジ反転）
6. **Offscreen_Blur** - ぼかし効果

## 基本的な使い方

### 1. ヘッダーファイルのインクルード

```cpp
#include "DirectXCommon.h"
#include "PSOType.h"
```

### 2. ポストエフェクトの設定

シーンの初期化やUpdateメソッド内で、DirectXCommonのポストエフェクトタイプを設定します：

```cpp
// 例：シーンのInitializeメソッド内
void MyScene::Initialize() {
    // グレースケールエフェクトを設定
    ctx_->dxCommon->SetPostEffectType(PSOType::Offscreen_Grayscale);
    
    // オフスクリーンレンダリングを有効化（デフォルトで有効）
    ctx_->dxCommon->SetRenderTextureEnabled(true);
}
```

### 3. 動的な切り替え

ImGuiを使用して実行時にエフェクトを切り替えることもできます：

```cpp
void MyScene::DrawImGui() {
#ifdef USE_IMGUI
    ImGui::Begin("Post Effect Settings");
    
    // 現在のエフェクトタイプを取得
    PSOType currentEffect = ctx_->dxCommon->GetPostEffectType();
    int currentIndex = static_cast<int>(currentEffect);
    
    // エフェクト名のリスト
    const char* effectNames[] = {
        "None (通常)",
        "Grayscale (白黒)",
        "Sepia (セピア調)",
        "Vignette (周辺減光)",
        "Invert (色反転)",
        "Blur (ぼかし)"
    };
    
    // コンボボックスでエフェクトを選択
    // 注意：PSOTypeの順序に依存しているため、
    // Offscreen系が連続している必要があります
    int selectedIndex = currentIndex - static_cast<int>(PSOType::Offscreen_None);
    
    if (ImGui::Combo("Post Effect", &selectedIndex, effectNames, IM_ARRAYSIZE(effectNames))) {
        // 選択されたエフェクトを設定
        PSOType newEffect = static_cast<PSOType>(
            static_cast<int>(PSOType::Offscreen_None) + selectedIndex
        );
        ctx_->dxCommon->SetPostEffectType(newEffect);
    }
    
    // オフスクリーンレンダリングのON/OFF切り替え
    bool useOffscreen = ctx_->dxCommon->IsRenderTextureEnabled();
    if (ImGui::Checkbox("Enable Post Effects", &useOffscreen)) {
        ctx_->dxCommon->SetRenderTextureEnabled(useOffscreen);
    }
    
    ImGui::End();
#endif
}
```

### 4. キー入力による切り替え例

```cpp
void MyScene::Update() {
    // 数字キー1～6でエフェクトを切り替え
    if (Input::TriggerKey(DIK_1)) {
        ctx_->dxCommon->SetPostEffectType(PSOType::Offscreen_None);
    }
    else if (Input::TriggerKey(DIK_2)) {
        ctx_->dxCommon->SetPostEffectType(PSOType::Offscreen_Grayscale);
    }
    else if (Input::TriggerKey(DIK_3)) {
        ctx_->dxCommon->SetPostEffectType(PSOType::Offscreen_Sepia);
    }
    else if (Input::TriggerKey(DIK_4)) {
        ctx_->dxCommon->SetPostEffectType(PSOType::Offscreen_Vignette);
    }
    else if (Input::TriggerKey(DIK_5)) {
        ctx_->dxCommon->SetPostEffectType(PSOType::Offscreen_Invert);
    }
    else if (Input::TriggerKey(DIK_6)) {
        ctx_->dxCommon->SetPostEffectType(PSOType::Offscreen_Blur);
    }
}
```

## 各エフェクトの説明

### Grayscale（グレースケール）
画面を白黒にします。NTSC係数を使用した加重平均により、人間の目の感度に基づいた自然な白黒変換を行います。

### Sepia（セピア調）
古い写真のような茶色がかった色調に変換します。ノスタルジックな雰囲気を演出できます。

### Vignette（ビネット）
画面の周辺を暗くする効果です。中央に視線を集中させたい場合に有効です。

### Invert（色反転）
RGB値を反転させてネガポジ反転のような効果を生み出します。

### Blur（ぼかし）
画面全体にぼかし効果を適用します。3x3のガウシアンブラーを使用しています。

## 注意事項

1. ポストエフェクトを使用するには、オフスクリーンレンダリングが有効である必要があります
2. 複数のエフェクトを同時に適用することはできません（1つのみ選択可能）
3. エフェクトの切り替えは、次のフレームから反映されます
4. パフォーマンスへの影響は各エフェクトによって異なります（Blurが最も重い）

## カスタムエフェクトの追加方法

新しいポストエフェクトを追加したい場合：

1. `PSOType.h`に新しいエフェクトタイプを追加
2. `resources/shaders/PostEffect/`に新しいピクセルシェーダーを作成
3. `PSOManager.cpp`の`CreatePSOOnDemand`メソッドに新しいケースを追加

詳細は既存のエフェクトの実装を参考にしてください。
