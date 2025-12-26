# Map3D クラス 使用方法と拡張ガイド

## 概要
`Map3D` は固定サイズの3Dブロックベースマップを管理するクラスです。  
座標系は **x:+方向**, **y:+方向(上)**, **z:+方向** で、ワールド原点 (0,0,0) は配列(0,0,0)ブロックの中心に対応します。

**ブロックサイズ**: デフォルトでは直径(10m, 5m, 10m)の非一様なサイズに対応しています。

---

## 基本的な使い方

### 1. マップの作成と初期化
```cpp
// 25x20x25 のマップを作成
auto map3D = std::make_unique<Map3D>(25, 20, 25);
map3D->Initialize();
```

### 2. タイルの設定
```cpp
// 座標 (x, y, z) に Normal ブロックを配置
// Normalブロックは直径(10m, 5m, 10m)のサイズで配置されます
map3D->SetTile(5, 0, 10, TileType::Normal);

// タイルを取得
TileType type = map3D->GetTile(5, 0, 10);

// 全てのタイルをクリア（Emptyにする）
map3D->Clear();
```

### 3. 描画
```cpp
void GameScene::Draw() {
    // カメラを渡して描画
    map3D->Draw(camera_);
}
```

### 4. ImGui表示
```cpp
void GameScene::DrawImGui() {
    map3D->DrawImGui();  // マップ情報を表示
}
```

---

## 座標変換

### ワールド座標 ⇔ マップ座標
```cpp
// マップ座標からワールド座標へ変換
// 配列(0,0,0) → ワールド(0,0,0)
// 配列(1,0,0) → ワールド(10,0,0)  // X軸は10m間隔
// 配列(0,1,0) → ワールド(0,5,0)   // Y軸は5m間隔
// 配列(0,0,1) → ワールド(0,0,10)  // Z軸は10m間隔
Vector3 worldPos = map3D->MapToWorld(10, 5, 15);

// ワールド座標からマップ座標へ変換
uint32_t x, y, z;
if (map3D->WorldToMap(playerPos, x, y, z)) {
    // プレイヤーの位置にあるタイルを取得
    TileType type = map3D->GetTile(x, y, z);
}
```

### ブロック配置の例
```cpp
// y=0 に床を配置
// Normalブロックは高さ5mなので、床の上面はy=2.5mの位置になります
map3D->SetTile(0, 0, 0, TileType::Normal);

// y=1 に次の段を配置
// この段の下面はy=2.5m、上面はy=7.5mの位置になります
map3D->SetTile(0, 1, 0, TileType::Normal);
```

---

## TileType の拡張方法

### 1. TileType に新しいタイプを追加
`Map3D.h` の `TileType` enum に追加します。

```cpp
enum class TileType : uint8_t {
    Empty = 0,
    Normal = 1,
    Wall = 2,      // 新規追加
    Lava = 3,      // 新規追加
    Ice = 4,       // 新規追加
};
```

### 2. モデルパスを登録
`Map3D.cpp` の `kModelPaths_` マップに追加します。

```cpp
const std::unordered_map<TileType, std::string> Map3D::kModelPaths_ = {
    { TileType::Normal, "MapBlock/NormalBlock.obj" },
    { TileType::Wall, "MapBlock/WallBlock.obj" },      // 新規追加
    { TileType::Lava, "MapBlock/LavaBlock.obj" },      // 新規追加
    { TileType::Ice, "MapBlock/IceBlock.obj" },        // 新規追加
};
```

### 3. スケールを設定
`Map3D.cpp` の `kTileScales_` マップに追加します。

```cpp
// Blenderモデルが半径1m（直径2m）のキューブの場合
// 直径10mにするには: スケール = 10 / 2 = 5.0
const std::unordered_map<TileType, Vector3> Map3D::kTileScales_ = {
    { TileType::Normal, { 5.0f, 2.5f, 5.0f } },  // 直径(10m, 5m, 10m)
    { TileType::Wall, { 1.0f, 3.0f, 1.0f } },    // 直径(2m, 6m, 2m) - 高い壁
    { TileType::Lava, { 5.0f, 0.5f, 5.0f } },    // 直径(10m, 1m, 10m) - 薄い溶岩
    { TileType::Ice, { 5.0f, 2.5f, 5.0f } },     // Normalと同じサイズ
};
```

これだけで、新しいタイプのブロックが自動的に描画されるようになります。

---

## ブロックサイズのカスタマイズ

### デフォルトのブロックサイズを変更する
`Map3D.h` のコンストラクタで `blockSize_` を変更します。

```cpp
// 例: すべてのブロックを 5x5x5m にする
Map3D::Map3D(uint32_t width, uint32_t height, uint32_t depth)
    : width_(width)
    , height_(height)
    , depth_(depth)
    , blockSize_{ 5.0f, 5.0f, 5.0f }  // ここを変更
{
    // ...
}
```

### タイプごとに異なるサイズを設定する
各タイプのスケールを `kTileScales_` で調整します。

```cpp
// 計算式: 希望する直径 / Blenderモデルの直径(2m)
// 例: 直径30mのブロック → スケール = 30 / 2 = 15.0

const std::unordered_map<TileType, Vector3> Map3D::kTileScales_ = {
    { TileType::Normal, { 5.0f, 2.5f, 5.0f } },   // 直径(10m, 5m, 10m)
    { TileType::Wall, { 0.5f, 5.0f, 0.5f } },     // 直径(1m, 10m, 1m) - 細くて高い柱
    { TileType::Lava, { 15.0f, 0.25f, 15.0f } },  // 直径(30m, 0.5m, 30m) - 広くて薄い
};
```

---

## メタデータの追加方法

タイルごとに追加情報（HP、ダメージ、スリップ効果など）を持たせたい場合は、以下のように拡張します。

### 1. メタデータ構造体を定義
`Map3D.h` に構造体を追加します。

```cpp
struct TileMetadata {
    int hp = 100;          // ブロックの耐久値
    float damage = 0.0f;   // 踏んだ時のダメージ
    bool slippery = false; // 滑るかどうか
};

struct BlockData {
    TileType type = TileType::Empty;
    std::unique_ptr<Model> model = nullptr;
    Transform transform;
    TileMetadata metadata;  // 新規追加
};
```

### 2. メタデータのアクセス用メソッドを追加
```cpp
class Map3D {
public:
    // メタデータの取得・設定
    TileMetadata& GetMetadata(uint32_t x, uint32_t y, uint32_t z) {
        return blocks_[ToIndex(x, y, z)].metadata;
    }

    void SetMetadata(uint32_t x, uint32_t y, uint32_t z, const TileMetadata& meta) {
        blocks_[ToIndex(x, y, z)].metadata = meta;
    }
};
```

### 3. 使用例
```cpp
// Lavaブロックにダメージ設定
map3D->SetTile(10, 0, 10, TileType::Lava);
auto& meta = map3D->GetMetadata(10, 0, 10);
meta.damage = 5.0f;

// 氷ブロックに滑り設定
map3D->SetTile(15, 0, 15, TileType::Ice);
map3D->GetMetadata(15, 0, 15).slippery = true;
```

---

## サンプルコード: 床と壁の生成

```cpp
// マップの作成
map3D_ = std::make_unique<Map3D>(25, 20, 25);
map3D_->Initialize();

// 床を作成（y=0の全面）
// 各Normalブロックは直径(10m, 5m, 10m)で配置されます
for (uint32_t z = 0; z < map3D_->GetDepth(); ++z) {
    for (uint32_t x = 0; x < map3D_->GetWidth(); ++x) {
        map3D_->SetTile(x, 0, z, TileType::Normal);
    }
}

// 外周に壁を作成（高さ5ブロック）
// Y軸方向に5ブロック = 5m × 5 = 25mの高さ
for (uint32_t y = 1; y < 5; ++y) {
    for (uint32_t z = 0; z < map3D_->GetDepth(); ++z) {
        map3D_->SetTile(0, y, z, TileType::Normal);  // X方向の壁
        map3D_->SetTile(map3D_->GetWidth() - 1, y, z, TileType::Normal);
    }
    for (uint32_t x = 0; x < map3D_->GetWidth(); ++x) {
        map3D_->SetTile(x, y, 0, TileType::Normal);  // Z方向の壁
        map3D_->SetTile(x, y, map3D_->GetDepth() - 1, TileType::Normal);
    }
}

// 中央に柱を配置（高さ10ブロック = 50m）
uint32_t centerX = map3D_->GetWidth() / 2;
uint32_t centerZ = map3D_->GetDepth() / 2;
for (uint32_t y = 1; y < 10; ++y) {
    map3D_->SetTile(centerX, y, centerZ, TileType::Normal);
}
```

---

## 座標計算の詳細

### ブロック配置の計算例
```cpp
// 配列座標 → ワールド座標の変換
// blockSize_ = { 10.0f, 5.0f, 10.0f }

配列(0, 0, 0) → ワールド(0, 0, 0)        // 原点
配列(1, 0, 0) → ワールド(10, 0, 0)       // X軸 +10m
配列(0, 1, 0) → ワールド(0, 5, 0)        // Y軸 +5m
配列(0, 0, 1) → ワールド(0, 0, 10)       // Z軸 +10m
配列(5, 3, 7) → ワールド(50, 15, 70)     // 組み合わせ
```

### ブロックのバウンディングボックス
```cpp
// Normalブロック (直径10m, 5m, 10m = 半径5m, 2.5m, 5m)
配列(0,0,0) のブロック:
  中心: (0, 0, 0)
  範囲: X [-5, +5], Y [-2.5, +2.5], Z [-5, +5]

配列(1,0,0) のブロック:
  中心: (10, 0, 0)
  範囲: X [5, 15], Y [-2.5, +2.5], Z [-5, +5]
```

---

## パフォーマンス最適化のヒント

### 1. フラスタムカリング
現在の実装では全ブロックを描画しますが、カメラ外のブロックを描画しないようにすることで大幅に最適化できます。

### 2. チャンクシステム
大きなマップの場合、チャンク（例: 16x16x16）単位で管理し、必要な部分だけ描画することで効率化できます。

### 3. 隣接ブロックの最適化
完全に囲まれたブロック（6面全てが他のブロックに接している）は描画しないようにすることで描画負荷を削減できます。

---

## まとめ

- **ブロックサイズ**: 各軸で異なるサイズに対応（デフォルト: 10m, 5m, 10m）
- **TileType 追加**: enum に値を追加 → モデルパスを登録 → スケールを設定
- **メタデータ追加**: 構造体を定義 → BlockDataに追加 → アクセサを実装
- **拡張性**: キャッシュ効率の良い1次元配列で管理しつつ、柔軟に機能追加可能

このクラスを基盤に、ゲームの要件に合わせて自由に拡張してください！
