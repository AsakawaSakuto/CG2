#pragma once
#include "Application/AppContext.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Bullet/bullet.h"
#include "Camera.h"

class Weapon {
public:
	void Initialize(AppContext* ctx);

	void Update();

	void Draw(Camera camera);
private:
	AppContext* ctx_;
	std::vector<std::unique_ptr<Bullet>> bullets_;
};