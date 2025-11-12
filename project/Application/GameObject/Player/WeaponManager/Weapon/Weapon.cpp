#include "Weapon.h"

void Weapon::Initialize(AppContext* ctx) {
	ctx_ = ctx;
}

void Weapon::Update() {
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
}

void Weapon::Draw(Camera camera) {
	for (auto& bullet : bullets_) {
		bullet->Draw(camera);
	}
}