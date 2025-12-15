# JsonManager 使用ガイド

## 概要
JsonManagerは、BinaryManagerと同様の機能を持つJSON形式のデータ保存・読み込みクラスです。
データを人間が読みやすいJSON形式で保存できます。

## 特徴
- BinaryManagerと同じインターフェース
- 自動的に`.json`拡張子を付与
- 人間が読める形式で保存
- Vector2, Vector3, Vector4などの型をサポート

## 使用方法

### 1. 基本的な使用例

```cpp
#include "Engine/Utility/Json/JsonManager.h"

// JsonManagerの初期化
auto jsonManager = std::make_unique<JsonManager>();
jsonManager->SetBasePath("resources/Json/MyData/");

// データの登録
Vector2 position = {100.0f, 200.0f};
Vector2 scale = {1.5f, 1.5f};
float speed = 5.0f;
Vector4 color = {1.0f, 0.5f, 0.0f, 1.0f};
bool isActive = true;

jsonManager->RegistOutput(position, "position");
jsonManager->RegistOutput(scale, "scale");
jsonManager->RegistOutput(speed, "speed");
jsonManager->RegistOutput(color, "color");
jsonManager->RegistOutput(isActive, "isActive");

// ファイルに保存（拡張子.jsonは自動で付与される）
jsonManager->Write("settings");  // settings.json として保存
```

### 2. データの読み込み

```cpp
// ファイルから読み込み（拡張子.jsonは自動で付与される）
auto values = jsonManager->Read("settings");  // settings.json から読み込み

if (!values.empty()) {
    size_t index = 0;
    if (index < values.size()) position = JsonManager::Reverse<Vector2>(values[index++]);
    if (index < values.size()) scale = JsonManager::Reverse<Vector2>(values[index++]);
    if (index < values.size()) speed = JsonManager::Reverse<float>(values[index++]);
    if (index < values.size()) color = JsonManager::Reverse<Vector4>(values[index++]);
    if (index < values.size()) isActive = JsonManager::Reverse<bool>(values[index++]);
}
```

### 3. BitmapFontでの使用例

```cpp
class BitmapFont {
private:
    std::unique_ptr<JsonManager> jsonManager_;
    
public:
    void Initialize() {
        jsonManager_ = std::make_unique<JsonManager>();
        jsonManager_->SetBasePath("resources/Json/BitmapFont/");
    }
    
    void SaveToJson(const std::string& filePath) {
        jsonManager_->RegistOutput(position_);
        jsonManager_->RegistOutput(scale_);
        jsonManager_->RegistOutput(spaceWidth_);
        jsonManager_->RegistOutput(color_);
        jsonManager_->RegistOutput(showPercent_);
        
        // 保存（拡張子は自動付与）
        jsonManager_->Write(filePath);
    }
    
    void LoadFromJson(const std::string& filePath) {
        // 読み込み（拡張子は自動付与）
        auto values = jsonManager_->Read(filePath);
        
        if (values.empty()) {
            printf("[WARNING] Failed to load json file: %s\n", filePath.c_str());
            return;
        }
        
        size_t index = 0;
        if (index < values.size()) position_ = JsonManager::Reverse<Vector2>(values[index++]);
        if (index < values.size()) scale_ = JsonManager::Reverse<Vector2>(values[index++]);
        if (index < values.size()) spaceWidth_ = JsonManager::Reverse<float>(values[index++]);
        if (index < values.size()) color_ = JsonManager::Reverse<Vector4>(values[index++]);
        if (index < values.size()) showPercent_ = JsonManager::Reverse<bool>(values[index++]);
    }
};
```

### 4. Particlesでの使用例

```cpp
class Particles {
private:
    std::unique_ptr<JsonManager> jsonManager_;
    
public:
    void Initialize() {
        jsonManager_ = std::make_unique<JsonManager>();
        jsonManager_->SetBasePath("resources/Json/Particle/");
    }
    
    void SaveToJson(const std::string& filePath) {
        jsonManager_->RegistOutput(emitter_.translate);
        jsonManager_->RegistOutput(emitter_.radius);
        jsonManager_->RegistOutput(emitter_.count);
        jsonManager_->RegistOutput(emitter_.frequency);
        // ... その他のパラメータ
        
        jsonManager_->Write(filePath);  // .json が自動付与
    }
    
    void LoadFromJson(const std::string& filePath) {
        auto values = jsonManager_->Read(filePath);  // .json が自動付与
        
        if (!values.empty()) {
            size_t index = 0;
            if (index < values.size()) emitter_.translate = JsonManager::Reverse<Vector3>(values[index++]);
            if (index < values.size()) emitter_.radius = JsonManager::Reverse<float>(values[index++]);
            // ... その他のパラメータ
        }
    }
};
```

## 生成されるJSONファイルの例

```json
{
  "values": [
    {
      "name": "position",
      "type": 16,
      "value": [100.000000, 200.000000]
    },
    {
      "name": "scale",
      "type": 16,
      "value": [1.500000, 1.500000]
    },
    {
      "name": "speed",
      "type": 2,
      "value": 5.000000
    },
    {
      "name": "color",
      "type": 18,
      "value": [1.000000, 0.500000, 0.000000, 1.000000]
    },
    {
      "name": "isActive",
      "type": 3,
      "value": true
    }
  ]
}
```

## サポートされている型

| 型 | TypeID | 説明 |
|---|---|---|
| int | 1 | 整数 |
| float | 2 | 浮動小数点数 |
| bool | 3 | 真偽値 |
| std::string | 4 | 文字列 |
| double | 5 | 倍精度浮動小数点数 |
| uint8_t | 6 | 8ビット符号なし整数 |
| uint32_t | 7 | 32ビット符号なし整数 |
| Vector2 | 16 | 2次元ベクトル |
| Vector3 | 17 | 3次元ベクトル |
| Vector4 | 18 | 4次元ベクトル |

## BinaryManagerとの比較

### JsonManagerの利点
- ? 人間が読める
- ? テキストエディタで編集可能
- ? バージョン管理しやすい
- ? デバッグしやすい

### BinaryManagerの利点
- ? ファイルサイズが小さい
- ? 読み込みが高速
- ? データの改ざんが困難

## 注意事項

1. **拡張子の自動付与**: ファイル名に`.json`をつけなくても自動的に付与されます
2. **ディレクトリの自動作成**: 存在しないディレクトリは自動的に作成されます
3. **順序の重要性**: 保存した順序と同じ順序で読み込む必要があります

## ImGuiでの使用例

```cpp
void MyClass::DrawImGui(const char* name) {
    ImGui::Begin(name);
    
    static char fileNameBuffer[256] = "temp";
    ImGui::InputText("ファイル名", fileNameBuffer, sizeof(fileNameBuffer));
    
    if (ImGui::Button("JSON保存")) {
        SaveToJson(fileNameBuffer);
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("JSON読み込み")) {
        LoadFromJson(fileNameBuffer);
    }
    
    ImGui::End();
}
```
