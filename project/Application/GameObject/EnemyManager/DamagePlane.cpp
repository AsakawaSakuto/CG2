#include "DamagePlane.h"

// 0-9   "resources/image/number/0-9.png"  
// 0 = { 0.0f,0.0f }, { 0.1f,1.0f } : 1 = { 0.1f,0.0f }, { 0.1f,1.0f } : 9 = { 0.9f,0.0f }, { 0.1f,1.0f }
// 10-19 "resources/image/number/10-19.png"
// 20-29 "resources/image/number/20-29.png"

void DamagePlane::Initialize(Vector3 pos, int damage) {
	damage_ = std::clamp(damage, 0, 99);

	model_ = std::make_unique<Model>();
	model_->Initialize("enemy/DamagePlane.obj");
	model_->SetTextureWithRect("resources/image/number/0-9.png", { 0.0f,0.0f }, { 0.1f,1.0f });

	transform_.translate = pos;
	transform_.scale = { 1.0f,1.0f,1.0f };
}

void DamagePlane::Update() {
	transform_.translate.y += 5.0f * 1.0f / 60.0f;
	transform_.rotate.y += 3.0f * 1.0f / 60.0f;
}

void DamagePlane::Draw(Camera camera) {
	model_->Draw(camera, transform_);
}