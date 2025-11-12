#pragma once
#include "Application/AppContext.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Weapon.h"
#include "Camera.h"

class WeaponManager {
public:
	void Initialize(AppContext* ctx);

	void Update();

	void Draw(Camera camera);
private:
	AppContext* ctx_;
	std::vector<std::unique_ptr<Weapon>> weapons_;
};