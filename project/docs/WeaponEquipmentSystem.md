# 武器装備システム

## 概要
Playerが後から武器を装備・変更・削除できるシステムです。武器スロットは最大4つまであり、各スロットに異なる武器を装備できます。

## 機能

### WeaponManager

#### 武器の装備
```cpp
bool EquipWeapon(WeaponName weaponName);
```
- 空いているスロットに新しい武器を装備します
- 既に同じ武器を持っている場合は装備できません
- スロットが満杯の場合は装備できません
- 戻り値: 装備に成功した場合 `true`、失敗した場合 `false`

#### 武器の変更
```cpp
bool ChangeWeapon(int slotIndex, WeaponName weaponName);
```
- 指定したスロットの武器を新しい武器に変更します
- `WeaponName::None` を指定すると武器を削除できます
- 戻り値: 変更に成功した場合 `true`、無効なスロット番号の場合 `false`

#### 武器の削除
```cpp
bool RemoveWeapon(int slotIndex);
```
- 指定したスロットの武器を削除します（`None` に変更）
- 戻り値: 削除に成功した場合 `true`、無効なスロット番号の場合 `false`

#### 武器の確認
```cpp
bool HasWeapon(WeaponName weaponName) const;
```
- 指定した武器を既に持っているか確認します
- 戻り値: 持っている場合 `true`、持っていない場合 `false`

```cpp
bool CanEquipWeapon() const;
```
- 武器を装備できる空きスロットがあるか確認します
- 戻り値: 空きスロットがある場合 `true`、満杯の場合 `false`

```cpp
int GetEmptySlotCount() const;
```
- 空きスロットの数を取得します
- 戻り値: 空きスロットの数（0-4）

### Player

Playerクラスから直接武器管理機能を呼び出せます：

```cpp
// 武器を装備
bool EquipWeapon(WeaponName weaponName);

// 武器を変更
bool ChangeWeapon(int slotIndex, WeaponName weaponName);

// 武器を持っているか確認
bool HasWeapon(WeaponName weaponName) const;

// 装備可能か確認
bool CanEquipWeapon() const;
```

## 使用例

### 1. 基本的な武器の装備

```cpp
// Laserを装備
if (player_->EquipWeapon(WeaponName::Laser)) {
    // 装備成功
    printf("Laser equipped!\n");
} else {
    // 装備失敗（スロット満杯 or 既に所持）
    printf("Cannot equip Laser\n");
}
```

### 2. 装備前の確認

```cpp
// 既に持っている武器かチェック
if (!player_->HasWeapon(WeaponName::Runa)) {
    // 空きスロットがあるかチェック
    if (player_->CanEquipWeapon()) {
        // 装備
        player_->EquipWeapon(WeaponName::Runa);
    } else {
        printf("No empty weapon slot!\n");
    }
} else {
    printf("Already have this weapon!\n");
}
```

### 3. 特定スロットの武器を変更

```cpp
// スロット1の武器をAxeに変更
if (player_->ChangeWeapon(1, WeaponName::Axe)) {
    printf("Weapon changed to Axe\n");
}

// スロット0の武器を削除
if (player_->ChangeWeapon(0, WeaponName::None)) {
    printf("Weapon removed\n");
}
```

### 4. WeaponManagerから直接操作

```cpp
auto* weaponManager = player_->GetWeaponManager();

// 空きスロット数を確認
int emptySlots = weaponManager->GetEmptySlotCount();
printf("Empty slots: %d\n", emptySlots);

// 武器を削除
weaponManager->RemoveWeapon(2); // スロット2の武器を削除
```

### 5. 宝箱から武器を獲得する例

```cpp
void OnChestOpened(WeaponName weaponReward) {
    // 既に持っているかチェック
    if (player_->HasWeapon(weaponReward)) {
        // 既に持っている場合は別の報酬を与える
        player_->AddMoney(100);
        ShowMessage("Already have this weapon! Got 100 gold instead.");
        return;
    }
    
    // 装備を試みる
    if (player_->EquipWeapon(weaponReward)) {
        ShowMessage("New weapon equipped!");
    } else {
        // スロットが満杯の場合
        ShowMessage("Weapon slots are full! Sell a weapon first.");
        // 報酬を保留するか、選択画面を表示するなど
    }
}
```

### 6. 武器ショップでの購入例

```cpp
void BuyWeapon(WeaponName weaponToBuy, int price) {
    // 既に持っているかチェック
    if (player_->HasWeapon(weaponToBuy)) {
        ShowMessage("You already have this weapon!");
        return;
    }
    
    // お金が足りるかチェック
    if (player_->GetNowMoney() < price) {
        ShowMessage("Not enough money!");
        return;
    }
    
    // 空きスロットがあるかチェック
    if (!player_->CanEquipWeapon()) {
        ShowMessage("No empty weapon slot! Sell a weapon first.");
        return;
    }
    
    // 購入処理
    if (player_->SubtractMoney(price) && player_->EquipWeapon(weaponToBuy)) {
        ShowMessage("Weapon purchased!");
    }
}
```

### 7. レベルアップ報酬での武器獲得

```cpp
void OnLevelUp(int newLevel) {
    WeaponName rewardWeapon = WeaponName::None;
    
    // レベルに応じて武器を解放
    switch (newLevel) {
        case 5:
            rewardWeapon = WeaponName::Laser;
            break;
        case 10:
            rewardWeapon = WeaponName::Runa;
            break;
        case 15:
            rewardWeapon = WeaponName::Axe;
            break;
    }
    
    if (rewardWeapon != WeaponName::None) {
        if (!player_->HasWeapon(rewardWeapon)) {
            if (player_->EquipWeapon(rewardWeapon)) {
                ShowMessage("Level up! New weapon unlocked!");
            } else {
                ShowMessage("Weapon unlocked, but slots are full!");
                // 後で装備できるようにアンロック情報を保存するなど
            }
        }
    }
}
```

## UIとの連携

武器アイコンは自動的に更新されます：

```cpp
// GameScene::UIUpdate() で自動的に呼ばれる
void GameScene::UIUpdate() {
    // ... 他のUI更新 ...
    
    // 武器アイコンの更新（自動）
    if (player_->GetWeaponManager()) {
        const auto& weapons = player_->GetWeaponManager()->GetWeapons();
        for (size_t i = 0; i < weapons.size() && i < 4; ++i) {
            if (weapons[i]) {
                WeaponName weaponName = weapons[i]->GetWeaponName();
                gameSceneUI_->UpdateWeaponIcon(static_cast<int>(i), weaponName);
            }
        }
    }
}
```

## 武器の種類

```cpp
enum class WeaponName {
    None,     // 武器なし
    FireBall, // ファイヤーボール（初期装備）
    Laser,    // レーザー
    Runa,     // ルナ
    Axe,      // アックス
    Count
};
```

## 注意事項

1. **重複チェック**: `EquipWeapon()` は同じ武器の重複装備を防ぎます
2. **スロット制限**: 最大4つまでの武器しか装備できません
3. **None は装備不可**: `WeaponName::None` は `EquipWeapon()` で装備できません（`ChangeWeapon()` では使用可能）
4. **自動UI更新**: 武器が変更されると、UIアイコンは自動的に更新されます

## 実装ファイル

- `Application/GameObject/Player/WeaponManager/WeaponManager.h`
- `Application/GameObject/Player/WeaponManager/WeaponManager.cpp`
- `Application/GameObject/Player/player.h`
- `Application/GameUI/GameSceneUI/GameSceneUI.h`
- `Application/GameUI/GameSceneUI/GameSceneUI.cpp`
