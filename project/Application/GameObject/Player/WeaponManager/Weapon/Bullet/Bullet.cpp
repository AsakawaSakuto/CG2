#include "bullet.h"

void Bullet::Initialize(AppContext* ctx) {
    ctx_ = ctx;

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.scale = { 0.5f,0.5f,0.5f };

	lifeTimer_.Start(5.0f, false);
}

void Bullet::Update() {

    transform_.translate += directionToEnemy_ * 25.0f * deltaTime_;

	if (particle_) {
		particle_->SetEmitterPosition(transform_.translate);
		particle_->Update();
	}

	// Sphere collider update
	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.5f; // Radius matched to bullet's size

	lifeTimer_.Update();
	if (lifeTimer_.IsFinished()) {
		Dead();
	}
}

void Bullet::Draw(Camera camera) {
	if (isAlive_ && lifeTimer_.GetDuration() >= 0.2f) {
		particle_->Draw(camera);
	}
}

void Bullet::DrawImGui() {

}

void Bullet::SetPosition(const Vector3& position) {
    transform_.translate = position;
}

void Bullet::SetDirectionToEnemy(const Vector3& direction) {
    directionToEnemy_ = direction.Normalize();
}