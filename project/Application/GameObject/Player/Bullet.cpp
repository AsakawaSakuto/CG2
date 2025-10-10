#include "Bullet.h"

void Bullet::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "player/bullet/bullet.obj");

	transform_.scale = { 1.0f, 1.0f, 1.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.translate = { 0.0f, 0.0f, 0.0f };
	collisionSphere_.center = transform_.translate;
	collisionSphere_.radius = 1.0f;
}

void Bullet::Update() {
	if (!isAlive_) {
		return;
	}

	// 位置更新
	transform_.translate.y += velocity_ * deltaTime_;

	collisionSphere_.center = transform_.translate;

	model_->SetTransform(transform_);
	model_->Update();
}

void Bullet::Draw(Camera useCamera) {
	if (!isAlive_) {
		return;
	}

	model_->Draw(useCamera); 
}

void Bullet::Spawn(Vector3 position, float velocity) {
	if (isAlive_) {
		return;
	} else {
		isAlive_ = true;
		transform_.translate = position;
		velocity_ = velocity;
	}
}