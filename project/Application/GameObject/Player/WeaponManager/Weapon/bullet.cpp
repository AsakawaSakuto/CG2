#include "bullet.h"

void Weapon::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	model_->Initialize(&ctx_->dxCommon, "weapon/bullet.obj");
}

void Weapon::Update() {

	model_->Update();
}

void Weapon::Draw(Camera camera) {

	model_->Draw(camera);
}

void Weapon::DrawImGui() {

}