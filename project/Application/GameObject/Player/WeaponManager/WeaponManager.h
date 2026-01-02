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

	// 武器のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<Weapon>>& GetWeapons() const { return weapons_; }

	void PostFrameCleanup();
private:
	std::vector<std::unique_ptr<Weapon>> weapons_;
	int maxWeaponNum_ = 4;
};