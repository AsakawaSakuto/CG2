#include "WeaponManager.h"

void WeaponManager::Initialize() {

	auto weapon = std::make_unique<Weapon>();
	weapon->Initialize(WeaponName::None);
	weapons_.push_back(std::move(weapon));

	auto weapon2 = std::make_unique<Weapon>();
	weapon2->Initialize(WeaponName::None);
	weapons_.push_back(std::move(weapon2));

	auto weapon3 = std::make_unique<Weapon>();
	weapon3->Initialize(WeaponName::None);
	weapons_.push_back(std::move(weapon3));

	auto weapon4 = std::make_unique<Weapon>();
	weapon4->Initialize(WeaponName::None);
	weapons_.push_back(std::move(weapon4));
}

void WeaponManager::Update() {
	for (auto& weapon : weapons_) {
		weapon->Update();
	}
}

void WeaponManager::Draw(Camera camera) {
	for (auto& weapon : weapons_) {
		weapon->Draw(camera);
	}
}

void WeaponManager::PostFrameCleanup() {
	for (auto& weapon : weapons_) {
		weapon->PostFrameCleanup();
	}
}

bool WeaponManager::EquipWeapon(WeaponName weaponName) {
	// Noneは装備できない
	if (weaponName == WeaponName::None) {
		return false;
	}

	// 既に同じ武器を持っている場合は装備しない
	if (HasWeapon(weaponName)) {
		return false;
	}

	// 空きスロットを探す
	for (size_t i = 0; i < weapons_.size(); ++i) {
		if (weapons_[i]->GetWeaponName() == WeaponName::None) {
			// 空きスロットに新しい武器を装備
			weapons_[i]->SetWeaponName(weaponName);
			return true;
		}
	}

	// スロットが満杯の場合
	return false;
}

bool WeaponManager::ChangeWeapon(int slotIndex, WeaponName weaponName) {
	// スロット番号の検証
	if (slotIndex < 0 || slotIndex >= static_cast<int>(weapons_.size())) {
		return false;
	}

	// 武器を変更（Noneも許可）
	auto newWeapon = std::make_unique<Weapon>();
	newWeapon->Initialize(weaponName);
	weapons_[slotIndex] = std::move(newWeapon);
	return true;
}

bool WeaponManager::RemoveWeapon(int slotIndex) {
	// スロット番号の検証
	if (slotIndex < 0 || slotIndex >= static_cast<int>(weapons_.size())) {
		return false;
	}

	// 武器をNoneに変更（削除）
	weapons_[slotIndex]->SetWeaponName(WeaponName::None);
	return true;
}

int WeaponManager::GetEmptySlotCount() const {
	int emptyCount = 0;
	for (const auto& weapon : weapons_) {
		if (weapon->GetWeaponName() == WeaponName::None) {
			emptyCount++;
		}
	}
	return emptyCount;
}

bool WeaponManager::CanEquipWeapon() const {
	return GetEmptySlotCount() > 0;
}

bool WeaponManager::HasWeapon(WeaponName weaponName) const {
	// Noneのチェックは無意味なので早期リターン
	if (weaponName == WeaponName::None) {
		return false;
	}

	for (const auto& weapon : weapons_) {
		if (weapon->GetWeaponName() == weaponName) {
			return true;
		}
	}
	return false;
}

int WeaponManager::GetWeaponKillCount(WeaponName weaponName) const {
	// Noneのチェックは無意味なので早期リターン
	if (weaponName == WeaponName::None) {
		return 0;
	}

	for (const auto& weapon : weapons_) {
		if (weapon->GetWeaponName() == weaponName) {
			return weapon->GetKillCount();
		}
	}
	return 0; // 武器を持っていない場合は0を返す
}

void WeaponManager::IncrementWeaponKillCount(WeaponName weaponName) {
	// Noneのチェックは無意味なので早期リターン
	if (weaponName == WeaponName::None) {
		return;
	}

	for (const auto& weapon : weapons_) {
		if (weapon->GetWeaponName() == weaponName) {
			weapon->IncrementKillCount();
			return; // 見つけたらインクリメントして終了
		}
	}
}

int WeaponManager::GetTotalWeaponKillCount() const {
	int totalKills = 0;
	for (const auto& weapon : weapons_) {
		if (weapon->GetWeaponName() != WeaponName::None) {
			totalKills += weapon->GetKillCount();
		}
	}
	return totalKills;
}