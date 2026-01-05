#pragma once
#include "GameObject/Player/WeaponManager/Weapon/Weapon.h"
#include "Camera/Camera.h"

class WeaponManager {
public:
	void Initialize();

	void Update();

	void Draw(Camera camera);

	void SetPlayerPosition(const Vector3& position) {
		for (auto& weapon : weapons_) {
			weapon->SetPlayerPosition(position);
		}
	}

	void SetDirectionToEnemy(const Vector3& direction) {
		for (auto& weapon : weapons_) {
			weapon->SetDirectionToEnemy(direction);
		}
	}

	void SetRandomDirectionToEnemy(const Vector3& direction) {
		for (auto& weapon : weapons_) {
			weapon->SetRandomDirectionToEnemy(direction);
		}
	}

	// 武器のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<Weapon>>& GetWeapons() const { return weapons_; }

	void PostFrameCleanup();

	/// <summary>
	/// 新しい武器を装備する
	/// </summary>
	/// <param name="weaponName">装備する武器の種類</param>
	/// <returns>装備に成功した場合true、スロットが満杯の場合false</returns>
	bool EquipWeapon(WeaponName weaponName);

	/// <summary>
	/// 指定スロットの武器を変更する
	/// </summary>
	/// <param name="slotIndex">変更するスロット番号（0-3）</param>
	/// <param name="weaponName">新しい武器の種類</param>
	/// <returns>変更に成功した場合true、無効なスロットの場合false</returns>
	bool ChangeWeapon(int slotIndex, WeaponName weaponName);

	/// <summary>
	/// 指定スロットの武器を削除する
	/// </summary>
	/// <param name="slotIndex">削除するスロット番号（0-3）</param>
	/// <returns>削除に成功した場合true、無効なスロットの場合false</returns>
	bool RemoveWeapon(int slotIndex);

	/// <summary>
	/// 空きスロットの数を取得
	/// </summary>
	int GetEmptySlotCount() const;

	/// <summary>
	/// 武器が装備可能か確認
	/// </summary>
	bool CanEquipWeapon() const;

	/// <summary>
	/// 指定の武器を持っているか確認
	/// </summary>
	bool HasWeapon(WeaponName weaponName) const;

	/// <summary>
	/// 指定した武器のキルカウントを取得
	/// </summary>
	/// <param name="weaponName">キルカウントを取得する武器の種類</param>
	/// <returns>指定した武器のキルカウント（武器を持っていない場合は0）</returns>
	int GetWeaponKillCount(WeaponName weaponName) const;

	/// <summary>
	/// 指定した武器のキルカウントをインクリメント
	/// </summary>
	/// <param name="weaponName">キルカウントをインクリメントする武器の種類</param>
	void IncrementWeaponKillCount(WeaponName weaponName);

	/// <summary>
	/// 全ての武器のキルカウントの合計を取得
	/// </summary>
	int GetTotalWeaponKillCount() const;

private:
	std::vector<std::unique_ptr<Weapon>> weapons_;
	int maxWeaponNum_ = 4;
};