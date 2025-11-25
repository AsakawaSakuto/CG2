#include "bullet.h"

void Bullet::Initialize(AppContext* ctx) {
    ctx_ = ctx;

    model_->Initialize(&ctx_->dxCommon, "ball.obj");

	particle_->Initialize(&ctx_->dxCommon);
	particle_->LoadJson("fireBall");

	transform_.scale = { 0.5f,0.5f,0.5f };
}

void Bullet::Update() {

    transform_.translate += directionToEnemy_ * 50.0f * deltaTime_;

    model_->Update();

	particle_->SetEmitterPosition(transform_.translate);
	particle_->Update();

	// Sphere collider update
	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.5f; // Radius matched to bullet's size
}

void Bullet::Draw(Camera camera) {

	particle_->Draw(camera);

    //model_->Draw(camera, transform_);
}

void Bullet::DrawImGui() {

}

void Bullet::SetPosition(const Vector3& position) {
    transform_.translate = position;
}

void Bullet::SetDirectionToEnemy(const Vector3& direction) {
    directionToEnemy_ = direction.Normalize();
}