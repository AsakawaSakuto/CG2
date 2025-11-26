#include "bullet.h"

void Bullet::Initialize(AppContext* ctx) {
    ctx_ = ctx;

	particle_->Initialize(&ctx_->dxCommon);
	particle_->LoadJson("fireBall");

	transform_.scale = { 0.5f,0.5f,0.5f };

	lifeTimer_.Start(5.0f, false);
}

void Bullet::Update() {

    transform_.translate += directionToEnemy_ * 50.0f * deltaTime_;

	particle_->SetEmitterPosition(transform_.translate);
	particle_->Update();

	// Sphere collider update
	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.5f; // Radius matched to bullet's size

	lifeTimer_.Update();
	if (lifeTimer_.IsFinished()) {
		Dead();
	}
}

void Bullet::Draw(Camera camera) {

	particle_->Draw(camera);
}

void Bullet::DrawImGui() {

}

void Bullet::SetPosition(const Vector3& position) {
    transform_.translate = position;
}

void Bullet::SetDirectionToEnemy(const Vector3& direction) {
    directionToEnemy_ = direction.Normalize();
}