#include "WeaponManager.h"

void WeaponManager::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	auto weapon = std::make_unique<Weapon>();
	weapon->Initialize(ctx_);
	weapons_.push_back(std::move(weapon));
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