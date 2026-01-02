# Dice武器の統合

## 概要
Diceクラス武器をWeaponManagerとCollisionManagerに統合し、ゲーム内で使用可能にしました。

## 変更内容

### 1. Weapon.h の変更
- `#include "Dice/Dice.h"` を追加
- `std::vector<std::unique_ptr<Dice>> dice_` メンバー変数を追加
- `GetDice()` ゲッター関数を追加
- `DiceUpdate()` プライベートメソッドを宣言

### 2. Weapon.cpp の変更

#### Initialize関数
- `WeaponName::Dice` ケースを追加
- Diceの初期ステータス設定:
  - クールダウン: 4.0秒
  - 間隔: 1.0秒
  - ショット数: 1
  - 基本ダメージ: 1.0（ランダムで1-6のダメージ）
  - ライフタイム: 10.0秒

#### Update関数
- `WeaponName::Dice` ケースを追加し、`DiceUpdate()` を呼び出し

#### Draw関数
- `DrawVec(dice_, camera)` を追加してDiceを描画

#### PostFrameCleanup関数
- `EraseDead(dice_)` を追加して死亡したDiceを削除

#### DiceUpdate関数（新規追加）
- クールダウンとインターバルタイマーの管理
- Diceの生成と初期化
- プレイヤー位置と敵への方向を設定
- Diceリストの更新

#### SetWeaponName関数
- `WeaponName::Dice` ケースを追加

### 3. CollisionManager.cpp の変更

#### CheckBulletEnemyCollision関数
- `const auto& dices = weapon->GetDice()` を追加してDiceリストを取得
- Diceと敵の衝突判定ループを追加:
  - 生存チェック
  - 球体衝突判定
  - ランダムダメージ（1-6）を`bullet->GetRandDamage()`で取得して適用
  - パーティクル効果の再生

### 4. Dice.cpp の修正
- `transform_.rotate * r * deltaTime_` → `transform_.rotate += r * deltaTime_` に修正（回転計算のバグ修正）
- スケールタイマーのロジック修正（`lifeTimer_` → `scaleTimer_`）

## Dice武器の特徴

### 基本仕様
- **ダメージ**: 1-6のランダムダメージ（サイコロの目に基づく）
- **クールダウン**: 4秒
- **寿命**: 10秒（発射後自動的に消滅）
- **挙動**: 
  - 直線移動
  - 回転しながら飛ぶ
  - 寿命の90%経過後、徐々に縮小して消滅

### ゲームプレイへの影響
- 不確実性のある武器として、リスク・リターンのバランスを提供
- 低ダメージ（1-2）から高ダメージ（5-6）まで幅広い結果が可能
- 長い寿命により、広範囲をカバー可能

## 関連ファイル
- `Application\GameObject\Player\WeaponManager\Weapon\Weapon.h`
- `Application\GameObject\Player\WeaponManager\Weapon\Weapon.cpp`
- `Application\GameObject\Player\WeaponManager\Weapon\Dice\Dice.h`
- `Application\GameObject\Player\WeaponManager\Weapon\Dice\Dice.cpp`
- `Application\CollisionManager\CollisionManager.cpp`
- `Application\GameObject\Player\WeaponManager\WeaponStatus.h` (既に`WeaponName::Dice`が定義済み)

## ビルド結果
? ビルドに成功しました

## 今後の拡張可能性
- ダメージの範囲を調整可能（例: 2-12など）
- クリティカル率の実装
- ダイスの目の視覚的表示
- 複数のダイスを同時発射する機能
