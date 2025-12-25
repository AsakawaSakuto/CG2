# AABB と OBB の min/max 方式への変更ドキュメント

## 概要
AABB（Axis-Aligned Bounding Box）と OBB（Oriented Bounding Box）の内部表現を、center + size 方式から min/max 方式に変更しました。

## 変更内容

### 1. AABB 構造体の変更 (`Engine/Utility/Collision/Type/AABB.h`)

#### 変更前:
```cpp
struct AABB {
    Vector3 center = { 0.0f, 0.0f, 0.0f }; // 中心点
    Vector3 size = { 1.0f, 1.0f, 1.0f };   // 各辺の長さ
};
```

#### 変更後:
```cpp
struct AABB {
    Vector3 min = { -0.5f, -0.5f, -0.5f }; // 最小点
    Vector3 max = { 0.5f, 0.5f, 0.5f };    // 最大点
    
    // ヘルパーメソッド
    Vector3 GetCenter() const;
    Vector3 GetSize() const;
};
```

### 2. OBB 構造体の変更 (`Engine/Utility/Collision/Type/OBB.h`)

#### 変更前:
```cpp
struct OBB {
    Vector3 center = { 0.0f, 0.0f, 0.0f }; // 中心点
    Vector3 size = { 1.0f, 1.0f, 1.0f };   // 各辺の長さ
    Vector3 rotate = { 0.0f, 0.0f, 0.0f }; // 回転角
    Vector3 orientation[3];                // 座標軸
};
```

#### 変更後:
```cpp
struct OBB {
    Vector3 min = { -0.5f, -0.5f, -0.5f }; // 最小点（ローカル空間）
    Vector3 max = { 0.5f, 0.5f, 0.5f };    // 最大点（ローカル空間）
    Vector3 rotate = { 0.0f, 0.0f, 0.0f }; // 回転角
    Vector3 orientation[3];                // 座標軸
    
    // ヘルパーメソッド
    Vector3 GetCenter() const;
    Vector3 GetSize() const;
};
```

### 3. 影響を受けるファイルの修正

#### 衝突判定 (`Engine/Utility/Collision/Collision.h`)
- `IsHit(const AABB& a, const AABB& b)`: min/max を直接使用
- `IsHit(const Sphere& sphere, const AABB& box)`: min/max を直接使用
- `IsHit(const AABB& aabb, const OBB& obb)`: min/max を使用してOBBに変換
- `IsHit(const OBB& obb, const Sphere& sphere)`: GetCenter() と GetSize() を使用
- `Detail::GetProjectionRadius()`: GetSize() を使用
- `Detail::IsSeparatedOnAxis()`: GetCenter() を使用
- その他の AABB/OBB 関連の衝突判定関数を更新

#### デバッグ描画 (`Engine/3d/Line/Data/Line.cpp`)
- `AddBox(const AABB& aabb, ...)`: min/max を直接使用
- `AddBox(const OBB& obb, ...)`: GetCenter() と GetSize() を使用

#### プレイヤー (`Application/GameObject/Player/Player.cpp`)
- `Initialize()`: AABB の初期化を min/max 方式に変更
- `Update()`: AABB の位置更新を min/max 方式に変更

## 使用方法

### AABB の作成

#### 方法1: 直接 min/max を指定
```cpp
AABB aabb;
aabb.min = { -1.0f, -1.0f, -1.0f };
aabb.max = { 1.0f, 1.0f, 1.0f };
```

#### 方法2: center と size から計算
```cpp
AABB aabb;
Vector3 center = { 0.0f, 1.0f, 0.0f };
Vector3 size = { 2.0f, 2.0f, 2.0f };
aabb.min = { center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f };
aabb.max = { center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f };
```

### OBB の作成

```cpp
OBB obb;
obb.min = { -0.5f, -0.5f, -0.5f };
obb.max = { 0.5f, 0.5f, 0.5f };
obb.rotate = { 0.0f, 0.785f, 0.0f }; // 45度回転
obb.UpdateOrientation();
```

### ヘルパーメソッドの使用

```cpp
AABB aabb;
aabb.min = { -1.0f, -1.0f, -1.0f };
aabb.max = { 1.0f, 1.0f, 1.0f };

Vector3 center = aabb.GetCenter(); // { 0.0f, 0.0f, 0.0f }
Vector3 size = aabb.GetSize();     // { 2.0f, 2.0f, 2.0f }
```

## メリット

1. **メモリ効率**: center と size を計算で求められるため、必要最小限のデータのみ保持
2. **衝突判定の最適化**: AABB × AABB 判定などで、center と size の計算が不要
3. **直感的**: 境界ボックスの範囲が min/max で明確に表現される
4. **互換性**: GetCenter() と GetSize() メソッドで既存コードとの互換性を維持

## 注意事項

1. ImGui での編集は min/max を直接編集する形式に変更
2. 既存のコードで center や size に直接アクセスしている箇所は、min/max または GetCenter()/GetSize() を使用するように修正が必要
3. OBB の min/max はローカル空間での値であることに注意

## 影響を受けるシステム

- 衝突判定システム (Collision.h)
- デバッグ描画システム (Line.cpp)
- プレイヤーシステム (Player.cpp)
- その他、AABB や OBB を使用している全てのコード

## ビルド結果

? ビルド成功 - すべての変更が正常にコンパイルされました
