# Toxic武器の統合

## 概要
Toxicクラス武器をWeaponManagerとCollisionManagerに統合し、ゲーム内で使用可能にしました。

## 変更内容

### 1. Weapon.h の変更
- `#include "Toxic/Toxic.h"` を追加
- `std::vector<std::unique_ptr<Toxic>> toxic_` メンバー変数を追加
- `GetToxic()` ゲッター関数を追加
- `ToxicUpdate()` プライベートメソッドを宣言

### 2. Weapon.cpp の変更

#### Initialize関数
- `WeaponName::Toxic` ケースを追加
- Toxicの初期ステータス設定:
  - クールダウン: 3.0秒
  - 間隔: 0.5秒
  - ショット数: 3（3発連射）
  - ダメージ: 2.0
  - ライフタイム: 5.0秒

#### Update関数
- `WeaponName::Toxic` ケースを追加し、`ToxicUpdate()` を呼び出し

#### Draw関数
- `DrawVec(toxic_, camera)` を追加してToxicを描画

#### PostFrameCleanup関数
- `EraseDead(toxic_)` を追加して死亡したToxicを削除

#### ToxicUpdate関数（新規追加）
- クールダウンとインターバルタイマーの管理
- Toxicの生成と初期化
- プレイヤー位置と敵への方向を設定
- 3発連射の実装
- Toxicリストの更新

#### SetWeaponName関数
- `WeaponName::Toxic` ケースを追加

### 3. CollisionManager.cpp の変更

#### CheckBulletEnemyCollision関数
- `const auto& toxics = weapon->GetToxic()` を追加してToxicリストを取得
- Toxicと敵の衝突判定ループを追加:
  - 生存チェック
  - 球体衝突判定
  - 通常ダメージ（2.0）を適用
  - パーティクル効果の再生
  - 将来的に継続ダメージ（DoT）の実装が可能

### 4. Toxic.cpp の修正
- スケールのLerp開始値を修正:
  - 変更前: `Vector3{ 1.0f, 1.0f, 1.0f }` から `{ 0.0f, 0.0f, 0.0f }` へ
  - 変更後: `Vector3{ 0.5f, 0.5f, 0.5f }` から `{ 0.0f, 0.0f, 0.0f }` へ
  - これにより、初期化時のスケール（0.5f）と一致

## Toxic武器の特徴

### 基本仕様
- **ダメージ**: 2.0（固定ダメージ）
- **クールダウン**: 3秒
- **発射数**: 3発連射（0.5秒間隔）
- **寿命**: 5秒（発射後自動的に消滅）
- **サイズ**: 通常武器の半分（スケール0.5）
- **色**: 紫色（RGB: 0.478, 0.188, 0.604）
- **挙動**: 
  - Y軸回転（毎フレーム6.0 * deltaTime）
  - 寿命の90%経過後、徐々に縮小して消滅

### ゲームプレイへの影響
- 連射武器として、複数の敵に対して効果的
- 小さいサイズながら3発連射で総ダメージ6.0を与えることが可能
- 中程度のクールダウンでバランスの取れた武器
- 紫色のビジュアルで毒属性を示唆

### 将来の拡張可能性
- **継続ダメージ（DoT）**: 当たった敵に毒状態を付与し、時間経過でダメージ
- **範囲効果**: 着弾時に毒の霧を発生させ、範囲ダメージ
- **スロー効果**: 当たった敵の移動速度を減少
- **スタック機能**: 複数の毒弾が当たると効果が累積

## 関連ファイル
- `Application\GameObject\Player\WeaponManager\Weapon\Weapon.h`
- `Application\GameObject\Player\WeaponManager\Weapon\Weapon.cpp`
- `Application\GameObject\Player\WeaponManager\Weapon\Toxic\Toxic.h`
- `Application\GameObject\Player\WeaponManager\Weapon\Toxic\Toxic.cpp`
- `Application\CollisionManager\CollisionManager.cpp`
- `Application\GameObject\Player\WeaponManager\WeaponStatus.h` (既に`WeaponName::Toxic`が定義済み)

## 技術的詳細

### 連射メカニズム
```cpp
status_.shotMaxCount = 3;      // 3発連射
status_.intervalTime = 0.5f;   // 0.5秒間隔
```
- インターバルタイマーが0.5秒ごとに発射
- `shotNowCount`が3に達するまで繰り返し
- 3発発射後にクールダウン（3秒）が開始

### スケールアニメーション
- 初期スケール: 0.5（通常の半分のサイズ）
- 寿命の90%経過後、縮小開始
- 線形補間でスムーズに0まで縮小

### 回転エフェクト
```cpp
transform_.rotate.y += 6.0f * deltaTime_;
```
- Y軸周りに継続的に回転
- デルタタイムで滑らかな回転を実現

## ビルド結果
? ビルドに成功しました

## まとめ
Toxic武器は、3発連射という特性を持つ毒属性の武器として実装されました。小さなサイズと紫色のビジュアルにより、他の武器と視覚的に区別できます。将来的には継続ダメージや範囲効果などの追加機能の実装が可能な設計になっています。
