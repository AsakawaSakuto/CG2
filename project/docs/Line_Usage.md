# Line クラス - 3D空間線描画

## 概要
`Line`クラスは、3D空間に頂点で直接線を描画するためのクラスです。デバッグ表示、UI、可視化などに使用できます。

## 特徴
- 頂点カラー対応（各線に色を設定可能）
- 深度テストの有効/無効を切り替え可能
- 便利なヘルパー関数を提供（グリッド、ボックス、球体、軸、円など）
- 効率的な動的頂点バッファ管理

## 基本的な使用方法

### 1. 初期化
```cpp
#include "Engine/3d/Line/Line.h"

// Lineクラスのインスタンスを作成
Line line;

// 初期化（DirectXCommonを渡す）
line.Initialize(&dxCommon);
```

### 2. 線の追加
```cpp
// 毎フレーム、前フレームの線をクリア
line.Clear();

// 2点を指定して線を追加
Vector3 start = { 0.0f, 0.0f, 0.0f };
Vector3 end = { 1.0f, 1.0f, 1.0f };
Vector4 color = { 1.0f, 0.0f, 0.0f, 1.0f }; // 赤色
line.AddLine(start, end, color);
```

### 3. 描画
```cpp
// カメラを渡して描画
line.Draw(camera);
```

## 便利な関数

### グリッド描画
```cpp
// 10x10のグリッドを描画（デフォルト: サイズ10、分割数10）
// 原点を通るX軸（Z=0の線）は赤色、Z軸（X=0の線）は緑色で自動的に描画されます
line.AddGrid();

// カスタマイズ例
line.AddGrid(20.0f, 20, { 0.3f, 0.3f, 0.3f, 1.0f }); // サイズ20、20分割、その他の線は暗いグレー
```

### ボックス（AABB）描画

中心とサイズで指定する方法：
```cpp
Vector3 center = { 0.0f, 1.0f, 0.0f };
Vector3 size = { 2.0f, 2.0f, 2.0f };
Vector4 color = { 0.0f, 1.0f, 0.0f, 1.0f }; // 緑色
line.AddBox(center, size, color);
```

AABBで指定する方法：
```cpp
AABB aabb;
aabb.min = { -1.0f, 0.0f, -1.0f };
aabb.max = { 1.0f, 2.0f, 1.0f };
Vector4 color = { 0.0f, 1.0f, 0.0f, 1.0f }; // 緑色
line.AddBox(aabb, color);
```

### 球体（ワイヤーフレーム）描画
```cpp
Vector3 center = { 0.0f, 0.0f, 0.0f };
float radius = 1.0f;
int segments = 16; // 分割数（滑らかさ）
Vector4 color = { 0.0f, 0.0f, 1.0f, 1.0f }; // 青色
line.AddSphere(center, radius, segments, color);
```

### 座標軸描画
```cpp
// 原点に座標軸を描画（X:赤、Y:緑、Z:青）
line.AddAxis();

// カスタマイズ例
Vector3 origin = { 5.0f, 0.0f, 0.0f };
float length = 2.0f;
line.AddAxis(origin, length);
```

### 円描画
```cpp
Vector3 center = { 0.0f, 2.0f, 0.0f };
float radius = 1.5f;
Vector3 normal = { 0.0f, 1.0f, 0.0f }; // Y軸方向（水平な円）
int segments = 32;
Vector4 color = { 1.0f, 1.0f, 0.0f, 1.0f }; // 黄色
line.AddCircle(center, radius, normal, segments, color);
```

### レイ（半直線）描画
```cpp
Vector3 origin = { 0.0f, 0.0f, 0.0f };
Vector3 direction = { 1.0f, 0.0f, 0.0f }; // X軸方向
float length = 5.0f;
Vector4 color = { 1.0f, 1.0f, 0.0f, 1.0f }; // 黄色
line.AddRay(origin, direction, length, color);
```

### 複数の線を一度に追加
```cpp
std::vector<Vector3> points = {
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 0.0f },
    { 2.0f, 0.0f, 0.0f },
    { 3.0f, 1.0f, 0.0f }
};
Vector4 color = { 1.0f, 0.0f, 1.0f, 1.0f }; // マゼンタ
line.AddLines(points, color); // 連続した線を描画
```

## 設定

### 深度テストの切り替え
```cpp
// 深度テストを無効にする（常に手前に描画）
line.SetDepthTest(false);

// 深度テストを有効にする（デフォルト）
line.SetDepthTest(true);
```

## 完全な使用例

```cpp
class GameScene {
private:
    Line debugLine_;
    Camera camera_;
    
public:
    void Initialize() {
        debugLine_.Initialize(&dxCommon);
    }
    
    void Update() {
        // 毎フレーム線をクリア
        debugLine_.Clear();
        
        // デバッグ用のグリッド表示
        debugLine_.AddGrid(20.0f, 20);
        
        // 座標軸表示
        debugLine_.AddAxis({ 0.0f, 0.0f, 0.0f }, 5.0f);
        
        // プレイヤーの当たり判定を可視化（中心とサイズで指定）
        Vector3 playerPos = player.GetPosition();
        debugLine_.AddBox(playerPos, { 1.0f, 2.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
        
        // 敵の当たり判定を可視化（AABBで指定）
        AABB enemyAABB = enemy.GetAABB();
        debugLine_.AddBox(enemyAABB, { 1.0f, 0.0f, 0.0f, 1.0f }); // 赤色
        
        // 敵の索敵範囲を可視化
        Vector3 enemyPos = enemy.GetPosition();
        debugLine_.AddCircle(enemyPos, 5.0f, { 0.0f, 1.0f, 0.0f }, 32, { 1.0f, 0.0f, 0.0f, 0.5f });
        
        // レイキャストの可視化
        Vector3 rayOrigin = camera.GetPosition();
        Vector3 rayDirection = camera.GetForward();
        debugLine_.AddRay(rayOrigin, rayDirection, 10.0f, { 1.0f, 1.0f, 0.0f, 1.0f });
    }
    
    void Draw() {
        // 3Dオブジェクトの描画...
        
        // 最後にライン描画（深度テスト無効で常に見えるようにする場合）
        debugLine_.Draw(camera_);
    }
};
```

## パフォーマンスに関する注意
- 大量の線を描画する場合、頂点バッファは自動的に拡張されますが、初期サイズ（デフォルト10000頂点）を超える場合は再作成が発生します
- 毎フレーム`Clear()`を呼び出して、不要な線をクリアすることを推奨します
- 固定的な線（グリッドなど）と動的な線（デバッグ用）を分けて描画することで、最適化できます

## 制限事項
- DirectX 12では線の幅は常に1ピクセル（`SetLineWidth()`は機能しません）
- アンチエイリアシングは使用できません
- 線の端の形状（キャップ）は制御できません

## 関連ファイル
- `Engine/3d/Line/Line.h` - ヘッダーファイル
- `Engine/3d/Line/Line.cpp` - 実装ファイル
- `resources/shaders/Line.VS.hlsl` - 頂点シェーダー
- `resources/shaders/Line.PS.hlsl` - ピクセルシェーダー
- `resources/shaders/Line.hlsli` - シェーダー共通定義
