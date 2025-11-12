#include "WeaponManager.h"

void WeaponManager::Initialize(AppContext* ctx) {
	ctx_ = ctx;
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