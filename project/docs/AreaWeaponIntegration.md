# Area武器の統合

## 概要
Areaクラス武器をWeaponManagerとCollisionManagerに統合し、ゲーム内で使用可能にしました。Areaは他の武器と異なり、常にプレイヤーの周囲に展開される範囲攻撃型の武器です。

## 変更内容

### 1. WeaponStatus.h の変更
- `WeaponName` enumに `Area` を追加

### 2. Weapon.h の変更
- `#include "Area/Area.h"` を追加
- `std::unique_ptr<Area> area_` メンバー変数を追加（単一インスタンス、vectorではない）
- `GetArea()` ゲッター関数を追加（const参照を返す）
- `AreaUpdate()` プライベートメソッドを宣言

### 3. Weapon.cpp の変更

#### Initialize関数
- `WeaponName::Area` ケースを追加
- Areaの初期ステータス設定:
  - ダメージ: 2.0
  - **クールダウン・インターバルなし**（常時発動）
- **初期化時にAreaインスタンスを生成**
  ```cpp
  area_ = std::make_unique<Area>();
  area_->Initialize();
  area_->SetDamage(status_.damage);
  ```

#### Update関数
- `WeaponName::Area` ケースを追加し、`AreaUpdate()` を呼び出し

#### Draw関数
- Area専用の描画処理を追加:
  ```cpp
  if (area_ && area_->IsAlive()) {
      area_->Draw(camera);
  }
  ```

#### PostFrameCleanup関数
- Areaは常時存在するため、クリーンアップ処理は不要

#### AreaUpdate関数（新規追加）
- **プレイヤー位置に追従**:
  ```cpp
  if (area_) {
      area_->SetPosition(playerPosition_);
      area_->Update();
  }
  ```
- 発射処理なし（常に存在）

#### SetWeaponName関数
- `WeaponName::Area` ケースを追加
- 初期化時と同じくAreaインスタンスを生成

### 4. CollisionManager.cpp の変更

#### CheckBulletEnemyCollision関数
- `const auto& area = weapon->GetArea()` でAreaを取得
- Areaの衝突判定を追加:
  - Areaが存在し、生存している場合のみ判定
  - Area範囲内の全ての敵に継続ダメージ
  - **パーティクル効果なし**（継続ダメージのため）
  - 敵の無敵時間を尊重

```cpp
if (area && area->IsAlive()) {
    const Sphere& areaSphere = area->GetSphereCollision();
    for (const auto& enemy : enemies) {
        if (!enemy->IsAlive()) continue;
        
        const Sphere& enemySphere = enemy->GetSphereCollision();
        if (Collision::IsHit(areaSphere, enemySphere) && !enemy->IsActiveInvincibleTimer()) {
            enemy->Damage(static_cast<int>(area->GetDamage()));
        }
    }
}
```

## Area武器の特徴

### 基本仕様
- **タイプ**: 範囲攻撃（常時発動）
- **ダメージ**: 2.0（毎フレーム判定）
- **範囲**: 球体コリジョン（半径1.0）
- **寿命**: 無限（武器を外すまで持続）
- **色**: ティール色（RGB: 0.094, 0.627, 0.576）
- **挙動**: 
  - Y軸回転（毎フレーム6.0 * deltaTime）
  - プレイヤーに追従
  - 常時展開

### 他の武器との違い

| 特徴 | 通常武器 | Area武器 |
|------|----------|----------|
| インスタンス | `std::vector<std::unique_ptr<T>>` | `std::unique_ptr<Area>` |
| 発射処理 | あり（クールダウン・インターバル） | なし（初期化時のみ生成） |
| 位置 | 発射時に固定、または移動 | 常にプレイヤーに追従 |
| 寿命 | あり（時間経過で消滅） | なし（常時存在） |
| クリーンアップ | 死亡弾を削除 | 不要 |

### ゲームプレイへの影響
- **防衛型武器**: プレイヤーの周囲を常に守る
- **継続ダメージ**: フレームごとにダメージ判定（実質DPS: 2.0 × 60 = 120.0/秒）
- **無敵時間の重要性**: 敵の無敵時間により、実際のダメージは調整される
- **接近戦サポート**: 近距離の敵を自動的に攻撃

### 技術的詳細

#### プレイヤー追従メカニズム
```cpp
void Weapon::AreaUpdate() {
    if (area_) {
        area_->SetPosition(playerPosition_);  // 毎フレーム位置更新
        area_->Update();
    }
}
```

#### 単一インスタンス設計
- **理由**: Areaは常に1つのみ存在
- **メリット**:
  - メモリ効率が良い
  - 管理が簡単
  - プレイヤーとの同期が容易

#### 継続ダメージの実装
- 敵の無敵タイマーとの組み合わせで実質的なDPSが決定
- パーティクルエフェクトを出さないことで、視覚的な混乱を防ぐ

### 将来の拡張可能性
- **範囲拡大**: スケールアップによる攻撃範囲の拡張
- **回転速度変更**: 視覚的なエフェクト強化
- **多段階発動**: レベルアップで複数のAreaを展開
- **属性追加**: 炎、氷、雷などの属性効果
- **デバフ効果**: 範囲内の敵の移動速度低下や攻撃力減少
- **パルス攻撃**: 一定時間ごとに大ダメージを与える

## 関連ファイル
- `Application\GameObject\Player\WeaponManager\Weapon\Weapon.h`
- `Application\GameObject\Player\WeaponManager\Weapon\Weapon.cpp`
- `Application\GameObject\Player\WeaponManager\Weapon\Area\Area.h`
- `Application\GameObject\Player\WeaponManager\Weapon\Area\Area.cpp`
- `Application\CollisionManager\CollisionManager.cpp`
- `Application\GameObject\Player\WeaponManager\WeaponStatus.h`

## 使用例

### 武器装備
```cpp
// プレイヤーにArea武器を装備
player->EquipWeapon(WeaponName::Area);
```

### 効果
- プレイヤーの周囲に常時展開される青緑色の回転する球体
- 範囲内の敵に毎フレームダメージ
- プレイヤーの移動に追従

## ビルド結果
? ビルドに成功しました

## まとめ
Area武器は、従来の発射型武器とは異なる「常時展開型」の範囲攻撃武器として実装されました。プレイヤーに追従し続けることで、近接戦闘のサポートや防衛的な役割を果たします。単一インスタンス設計により、効率的なメモリ管理と簡潔なコード構造を実現しています。

### 設計のポイント
1. **単一インスタンス**: vectorではなく`unique_ptr`で管理
2. **初期化時生成**: 武器装備時に即座に生成、発射処理なし
3. **プレイヤー追従**: 毎フレーム位置を更新
4. **継続ダメージ**: パーティクルなしで視覚的にクリーン
5. **無敵時間尊重**: 他の武器と同様の判定ロジック
