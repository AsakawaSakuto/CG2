#include "Axe.h"

void Axe::Initialize() {

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.scale = { 1.0f,1.0f,1.0f };

	model_->Initialize("weapon/axe/Axe.obj");
	moveTimer_.Start(1.0f, false);

	trailParticle_->Initialize("axe");
	trail2Particle_->Initialize("axe");
}

void Axe::Update() {

	if (moveTimer_.IsActive()) {
		moveTimer_.Update();
		transform_.translate += directionToEnemy_ * speed_ * deltaTime_;
	}

	if (moveTimer_.IsFinished() && !lifeTimer_.IsActive()) {
		lifeTimer_.Start(lifeTime_, false);
	}

	trailParticle_->SetEmitterPosition(model_->GetVertexWorldPosition(1143));
	trail2Particle_->SetEmitterPosition(model_->GetVertexWorldPosition(1527));
	trailParticle_->Update();
	trail2Particle_->Update();

	sphereCollision_.center = transform_.translate;
	transform_.rotate.y += 4.00f * deltaTime_;
	sphereCollision_.radius = 1.0f;

	MyDebugLine::AddShape(sphereCollision_);

	lifeTimer_.Update();

	if (lifeTimer_.GetProgress() >= 0.9f) {
		if (!scaleTimer_.IsActive()) {
			scaleTimer_.Start(lifeTime_ / 10.0f, false);
		}
	}

	if (lifeTimer_.IsActive()) {
		lifeTimer_.Update();
		transform_.scale = MyEasing::Lerp(Vector3{ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, lifeTimer_.GetProgress(), EaseType::Linear);
	}

	if (lifeTimer_.IsFinished()) {
		Dead();
	}
}

void Axe::Draw(Camera camera) {
	if (isAlive_) {
		model_->Draw(camera, transform_);
		trailParticle_->Draw(camera);
		trail2Particle_->Draw(camera);
	}
}

void Axe::DrawImGui() {

}