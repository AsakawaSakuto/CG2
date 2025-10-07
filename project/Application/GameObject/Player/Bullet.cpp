#include "Bullet.h"

void Bullet::Initialize(DirectXCommon* dxCommon) {
}

void Bullet::Initialize(DirectXCommon* dxCommon, Vector3 translate) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/model/player/player.obj");

	transform_.scale = {0.5f, 0.5f, 0.5f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = translate;
	CollitionSphere_.center = transform_.translate;
	CollitionSphere_.radius = 1.0f;
}

void Bullet::Update() {
	// 位置更新
	transform_.translate.y += velocity_ * deltaTime_;
}

void Bullet::Draw(Camera useCamera) { model_->Draw(useCamera); }

void Bullet::Draw(Camera useCamera, Model* model) { model->Draw(useCamera); }

void Bullet::SetVelocity(float velocity) { velocity_ = velocity; }
