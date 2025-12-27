# Map3D AABB衝突システム - ビジュアルガイド

## AABB構造の図解

```
プレイヤーAABB:
    center = transform_.translate (プレイヤー位置)
    min = { -0.5, 0.0, -0.5 }  (ローカルオフセット)
    max = { 0.5, 2.0, 0.5 }    (ローカルオフセット)

    ワールド座標:
         max.y (頭)
          ↑
          |  +---------+
          |  |         |  ← 幅: 1.0m (min.x～max.x)
    2.0m  |  |    P    |  ← center (プレイヤー位置)
          |  |         |
          |  +---------+
          ↓
         min.y (足元)
         
    足元のY座標 = center.y + min.y = center.y + 0.0
    頭のY座標   = center.y + max.y = center.y + 2.0
```

```
ブロックAABB (TileType::Normal):
    center = block.transform.translate (ブロック中心)
    min = { -5.0, -2.5, -5.0 }  (ローカルオフセット)
    max = { 5.0, 2.5, 5.0 }     (ローカルオフセット)
    
    blockSize_ = { 10.0, 5.0, 10.0 }  (直径)

    ワールド座標:
         max.y (上面)
          ↑
          |  +-------------------+
    2.5m  |  |                   |  ← 幅: 10.0m
          |  |        B          |  ← center (ブロック中心)
          |  |                   |  ← 高さ: 5.0m
    2.5m  |  +-------------------+
          ↓
         min.y (下面)
         
    下面のY座標 = center.y + min.y = center.y - 2.5
    上面のY座標 = center.y + max.y = center.y + 2.5
```

## 衝突シナリオの図解

### シナリオ1: 床に着地（上から落下）

```
落下前:
    Y
    ↑
    |    [P]  ← プレイヤー（落下中、velY < 0）
    |     
    |     
    |  +-----+
    |  |  B  |  ← ブロック
    |  +-----+
    +----------→ X

重なり検出:
    playerMin.y < blockMax.y  ?
    playerMax.y > blockMin.y  ?
    
    overlapBottom = playerMax.y - blockMin.y
    (プレイヤー下部がブロック下面に侵入している量)

押し戻し後:
    Y
    ↑
    |    [P]  ← プレイヤー（ブロックの上に乗る）
    |  ------  ← blockMax.y (上面)
    |  +-----+
    |  |  B  |
    |  +-----+
    +----------→ X
    
    最終調整:
    playerPos.y = blockMax.y - playerAabb.min.y
    (プレイヤーの足を床の上面にぴったり合わせる)
    
    結果:
    - isGrounded = true
    - velocity_Y = 0
    - ジャンプカウントリセット
```

### シナリオ2: 天井に当たる（下から上昇）

```
上昇前:
    Y
    ↑
    |  +-----+
    |  |  B  |  ← 天井ブロック
    |  +-----+
    |     
    |    [P]  ← プレイヤー（上昇中、velY > 0）
    |     
    +----------→ X

重なり検出:
    playerMin.y < blockMax.y  ?
    playerMax.y > blockMin.y  ?
    
    overlapTop = blockMax.y - playerMin.y
    (プレイヤー上部がブロック下面に侵入している量)

押し戻し後:
    Y
    ↑
    |  +-----+
    |  |  B  |
    |  +-----+
    |  ------  ← blockMin.y (下面)
    |    [P]  ← プレイヤー（押し戻された）
    |     
    +----------→ X
    
    結果:
    - playerPos.y -= overlapTop (下に押し戻し)
    - velocity_Y = 0 (上昇を止める)
```

### シナリオ3: 壁に当たる（側面から）

```
X軸の衝突:
    Z
    ↑
    |         +-----+
    |    [P]→ |  B  |  ← 壁ブロック
    |         +-----+
    +--------------------→ X

重なり検出:
    playerMin.x < blockMax.x  ?
    playerMax.x > blockMin.x  ?
    
    overlapRight = blockMax.x - playerMin.x  (左から侵入)
    overlapLeft = playerMax.x - blockMin.x   (右から侵入)
    
    overlapX = (overlapRight < overlapLeft) ? -overlapRight : overlapLeft

押し戻し後:
    Z
    ↑
    |   [P]   +-----+
    |    ←←   |  B  |
    |         +-----+
    +--------------------→ X
    
    結果:
    - playerPos.x += overlapX (左に押し戻し)
```

## 近傍セル計算の図解

```
マップ全体（25x20x25）:
    
    プレイヤーのAABB範囲:
    playerMin = { 5.3, 0.0, 7.2 }
    playerMax = { 6.3, 2.0, 8.2 }
    
    セル範囲の計算:
    minCellX = floor(5.3 / 10.0) = 0
    maxCellX = floor(6.3 / 10.0) = 0
    minCellZ = floor(7.2 / 10.0) = 0
    maxCellZ = floor(8.2 / 10.0) = 0
    
    チェックするセル:
    (0, y, 0) のみ
    
    ↓ プレイヤーが移動して複数セルにまたがる場合:
    
    playerMin = { 9.8, 0.0, 9.8 }
    playerMax = { 10.8, 2.0, 10.8 }
    
    minCellX = floor(9.8 / 10.0) = 0
    maxCellX = floor(10.8 / 10.0) = 1
    minCellZ = floor(9.8 / 10.0) = 0
    maxCellZ = floor(10.8 / 10.0) = 1
    
    チェックするセル:
    (0, y, 0), (1, y, 0), (0, y, 1), (1, y, 1)
    
    マップビュー（上から見た図）:
    Z
    ↑
    |  +-----+-----+-----+
    |  |     |     |     |
 1  |  | (0,1) (1,1)   |
    |  |     | [P] |     |  ← プレイヤーが4セルにまたがる
    |  +-----+-----+-----+
    |  |     | [P] |     |
 0  |  | (0,0) (1,0)   |
    |  |     |     |     |
    |  +-----+-----+-----+
    +----0-----1-----2-----→ X
    
    効率的な処理:
    - 総当たり: 25 x 20 x 25 = 12,500セル
    - 近傍セルのみ: 最大4セル（大幅に削減）
```

## Update()の実行フロー

```
Player::Update() の処理順序:

1. wasGrounded_ = isGrounded_
   ├─ 前フレームの地面状態を保存
   └─ 着地判定に使用

2. Move()
   ├─ 入力を取得（WASD/スティック）
   ├─ カメラ方向から移動ベクトルを計算
   └─ transform_.translate.x/z を更新
       （XZ平面のみ、Y軸は変更なし）

3. Jump()
   ├─ ジャンプ入力チェック（Aボタン）
   │   └─ velocity_Y = jumpPower
   ├─ 重力を適用
   │   └─ velocity_Y -= gravity * deltaTime
   └─ Y軸位置を更新（仮）
       └─ transform_.translate.y += velocity_Y * deltaTime

4. Map3D::ResolvePlayerAABBCollision()
   ├─ 近傍セル範囲を計算
   │   └─ playerAABB範囲 → セル範囲
   │
   ├─ Y軸の衝突解決（優先）
   │   ├─ 各Normal ブロックとの判定
   │   ├─ 床判定（velY <= 0）
   │   │   ├─ 上に押し戻し
   │   │   ├─ velocity_Y = 0
   │   │   └─ isGrounded = true
   │   └─ 天井判定（velY > 0）
   │       ├─ 下に押し戻し
   │       └─ velocity_Y = 0
   │
   ├─ X/Z軸の衝突解決
   │   ├─ Y軸解決後の位置で再計算
   │   ├─ 各Normalブロックとの判定
   │   ├─ X軸の重なり量を計算
   │   ├─ Z軸の重なり量を計算
   │   └─ 最小の押し戻しを適用
   │       └─ abs(overlapX) < abs(overlapZ) ? X : Z
   │
   └─ 最終調整
       └─ playerPos.y = highestFloorY - playerAabb.min.y
           （床にぴったり合わせる）

5. 着地判定
   ├─ if (isGrounded && !wasGrounded)
   │   ├─ landingParticle_->Play()
   │   └─ currentJumpCount_ = 0
   └─ 着地した瞬間の処理

6. mapCollosion_.center = transform_.translate
   └─ 最終位置でAABBを更新

7. その他の処理（武器、パーティクルなど）
```

## 衝突解決の詳細フロー

```
ResolvePlayerAABBCollision() の内部処理:

入力:
├─ playerPos (プレイヤー位置)
├─ playerVelY (Y軸速度)
├─ playerAabb (プレイヤーAABB)
└─ isGrounded (地面フラグ)

STEP 1: 近傍セル計算
├─ playerAabb.center = playerPos
├─ playerMin = playerAabb.GetMinWorld()
├─ playerMax = playerAabb.GetMaxWorld()
├─ セル範囲 = floor(playerMin/Max / blockSize)
└─ クランプ(0 ～ mapSize-1)

STEP 2: Y軸解決ループ
for each cell in 近傍セル範囲:
    if (cell.type != Normal) continue
    
    if (Collision::IsHit(playerAabb, blockAabb)):
        ├─ blockMin = block.aabb.GetMinWorld()
        ├─ blockMax = block.aabb.GetMaxWorld()
        │
        ├─ overlapTop = blockMax.y - playerMin.y
        ├─ overlapBottom = playerMax.y - blockMin.y
        │
        ├─ if (overlapTop < overlapBottom):
        │   └─ 天井判定 (velY > 0)
        │       ├─ overlapY = -overlapTop
        │       └─ playerVelY = 0
        │
        └─ else:
            └─ 床判定 (velY <= 0)
                ├─ overlapY = overlapBottom
                ├─ playerVelY = 0
                ├─ isGrounded = true
                └─ highestFloorY = blockMax.y

    ├─ playerPos.y += overlapY
    ├─ playerAabb.center = playerPos
    └─ 範囲を再計算

└─ if (foundFloor):
    ├─ isGrounded = true
    └─ playerPos.y = highestFloorY - playerAabb.min.y

STEP 3: XZ軸解決ループ
├─ 近傍セルを再計算（Y軸移動後）
│
for each cell in 近傍セル範囲:
    if (cell.type != Normal) continue
    
    if (Collision::IsHit(playerAabb, blockAabb)):
        ├─ X軸の重なり計算
        │   ├─ overlapRight = blockMax.x - playerMin.x
        │   ├─ overlapLeft = playerMax.x - blockMin.x
        │   └─ overlapX = min(overlapRight, overlapLeft)
        │
        ├─ Z軸の重なり計算
        │   └─ 同様...
        │
        └─ 最小押し戻し適用
            if (abs(overlapX) < abs(overlapZ)):
                playerPos.x += overlapX
            else:
                playerPos.z += overlapZ
        
        ├─ playerAabb.center = playerPos
        └─ 範囲を再計算

出力:
├─ playerPos (更新された位置)
├─ playerVelY (更新された速度)
└─ isGrounded (更新された地面フラグ)
```

## コーディング規約とベストプラクティス

### 1. AABB更新のタイミング
```cpp
// ? 間違い: centerを更新せずに使用
transform_.translate = newPosition;
// mapCollosion_.centerは古いまま！

// ? 正しい: centerを即座に更新
transform_.translate = newPosition;
mapCollosion_.center = transform_.translate;
```

### 2. 衝突解決の順序
```cpp
// ? 間違い: すべての軸を同時に解決
for (auto& block : blocks) {
    if (collides) {
        playerPos.x += overlapX;
        playerPos.y += overlapY;  // 床判定が不安定に
        playerPos.z += overlapZ;
    }
}

// ? 正しい: Y軸を優先、次にX/Z
// STEP 1: Y軸のみ解決
for (auto& block : blocks) {
    if (collides) {
        playerPos.y += overlapY;
        playerVelY = 0;
    }
}

// STEP 2: X/Z軸を解決
for (auto& block : blocks) {
    if (collides) {
        // 最小の押し戻しを適用
    }
}
```

### 3. 速度の処理
```cpp
// ? 間違い: 速度を保持したまま
if (collides) {
    playerPos.y = blockTop;
    // velocity_Y はそのまま → 次フレームでまた侵入
}

// ? 正しい: 衝突時は速度をゼロに
if (collides) {
    playerPos.y = blockTop;
    playerVelY = 0.0f;  // 必ず速度をゼロに
}
```

### 4. 近傍セルの計算
```cpp
// ? 間違い: 総当たり（パフォーマンス問題）
for (int x = 0; x < mapWidth; x++) {
    for (int y = 0; y < mapHeight; y++) {
        for (int z = 0; z < mapDepth; z++) {
            // 全セルをチェック（12,500セル！）
        }
    }
}

// ? 正しい: 近傍セルのみ
int minX = floor(playerMin.x / blockSize.x);
int maxX = floor(playerMax.x / blockSize.x);
// クランプして範囲を制限
for (int x = minX; x <= maxX; x++) {
    // 最大4セル程度
}
```

## トラブルシューティング

### 問題1: プレイヤーが床をすり抜ける
**原因**: Y軸の衝突解決が正しく動作していない

**チェックポイント**:
1. `mapCollosion_.center`が正しく更新されているか
2. `playerVelY <= 0.0f`の条件が正しいか
3. `isGrounded`が正しく設定されているか

**解決策**:
```cpp
// デバッグ表示を追加
ImGui::Text("PlayerY: %.2f", transform_.translate.y);
ImGui::Text("VelY: %.2f", status_.velocity_Y_);
ImGui::Text("IsGrounded: %s", isGrounded_ ? "Yes" : "No");
```

### 問題2: プレイヤーが壁にめり込む
**原因**: X/Z軸の衝突解決が動作していない

**チェックポイント**:
1. Y軸解決後にセル範囲を再計算しているか
2. 最小押し戻しのロジックが正しいか

**解決策**:
```cpp
// AABBをデバッグ表示
MyDebugLine::AddShape(playerAabb, {1.0f, 0.0f, 0.0f, 1.0f});
```

### 問題3: ジャンプが効かない
**原因**: 地面判定が常にtrueになっている

**チェックポイント**:
1. 空中で`isGrounded`がfalseになっているか
2. `Jump()`が呼ばれる順序は正しいか

**解決策**:
```cpp
// Update()の順序を確認
wasGrounded_ = isGrounded_;
Move();        // XZ移動
Jump();        // Y軸速度更新
// 衝突解決でisGrounded_が更新される
```

### 問題4: 天井に張り付く
**原因**: 天井判定で速度がゼロになっていない

**解決策**:
```cpp
// 天井判定時
if (playerVelY > 0.0f) {
    playerPos.y -= overlapTop;
    playerVelY = 0.0f;  // ← これが重要！
}
```

## パフォーマンス最適化のヒント

### 1. 近傍セルの計算を最適化
```cpp
// セル範囲を1回だけ計算
struct CellRange {
    int minX, maxX;
    int minY, maxY;
    int minZ, maxZ;
};

CellRange CalculateNearbyCell(const AABB& aabb) {
    // 計算を1箇所にまとめる
}
```

### 2. 早期リターンを活用
```cpp
// 空中で遠く離れている場合はスキップ
if (!isGrounded && transform_.translate.y > mapMaxHeight + 10.0f) {
    return;
}
```

### 3. 衝突判定の最適化
```cpp
// AABBの大まかなチェックを先に
float distance = Length(playerPos - blockPos);
if (distance > safeDistance) continue;  // 遠すぎる場合はスキップ

// 詳細な判定
if (Collision::IsHit(playerAabb, blockAabb)) {
    // 衝突解決
}
```
