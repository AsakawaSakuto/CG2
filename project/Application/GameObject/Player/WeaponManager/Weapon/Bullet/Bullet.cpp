#include "bullet.h"

void Bullet::Initialize(AppContext* ctx) {
    ctx_ = ctx;
    model_->Initialize(&ctx->dxCommon, "cube.obj");

	transform_.scale = { 0.25f,0.25f,0.25f };
}

void Bullet::Update() {

    transform_.translate += directionToEnemy_ * 100.0f * deltaTime_;

    model_->Update();

	// Sphere collider update
	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.25f; // Radius matched to bullet's size
}

void Bullet::Draw(Camera camera) {

    model_->Draw(camera, transform_);
}

void Bullet::DrawImGui() {

}

void Bullet::SetPosition(const Vector3& position) {
    transform_.translate = position;
}

void Bullet::SetDirectionToEnemy(const Vector3& direction) {
    directionToEnemy_ = direction.Normalize();
}