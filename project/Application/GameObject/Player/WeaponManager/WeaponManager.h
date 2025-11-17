#pragma once
#include "Application/AppContext.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Weapon.h"
#include "Camera.h"

class WeaponManager {
public:
	void Initialize(AppContext* ctx);

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
private:
	AppContext* ctx_;
	std::vector<std::unique_ptr<Weapon>> weapons_;
};