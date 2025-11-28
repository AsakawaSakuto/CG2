#include "bullet.h"

void Bullet::Initialize(AppContext* ctx) {
    ctx_ = ctx;

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.scale = { 0.5f,0.5f,0.5f };

	lifeTimer_.Start(10.0f, false);

	particle_->Initialize(&ctx_->dxCommon);
	particle2_->Initialize(&ctx_->dxCommon);
}

void Bullet::Update() {

	if (penetrationCount_ > 0) {
		bulletType_ = BulletType::Penetration;
	} else if (bounceCount_ > 0) {
		bulletType_ = BulletType::Bounce;
	} else {
		bulletType_ = BulletType::None;
	}

    transform_.translate += directionToEnemy_ * speed_ * deltaTime_;

	// Sphere collider update
	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.5f;

	
	particle_->SetEmitterPosition(transform_.translate);
	particle_->Update();

	particle2_->SetEmitterPosition(transform_.translate);
	particle2_->Update();


	lifeTimer_.Update();

	if (lifeTimer_.GetProgress() >= 0.9f) {
		particle_->Stop();
		particle2_->Stop();
	}

	if (lifeTimer_.IsFinished()) {
		Dead();
	}
}

void Bullet::Draw(Camera camera) {
	if (isAlive_ && lifeTimer_.GetDuration() >= 0.2f) {
		particle_->Draw(camera);
		particle2_->Draw(camera);
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