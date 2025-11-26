#include "WeaponManager.h"
#include "Application/GameObject/Player/WeaponManager/WeaponName.h"

void WeaponManager::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	auto weapon = std::make_unique<Weapon>();
	weapon->Initialize(ctx_, WeaponName::FireBall);
	weapons_.push_back(std::move(weapon));

	auto weapon2 = std::make_unique<Weapon>();
	weapon2->Initialize(ctx_, WeaponName::Laser);
	weapons_.push_back(std::move(weapon2));
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