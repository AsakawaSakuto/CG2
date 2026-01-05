#include "Boomerang.h"

void Boomerang::Initialize() {

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.scale = { 1.0f,1.0f,1.0f };

	model_->Initialize("weapon/Boomerang/Boomerang.obj");
	
	trailParticle_->Initialize("axe");
	trail2Particle_->Initialize("axe");

	lifeTimer_.Start(lifeTime_, false);
	goTimer_.Start(3.0f, false);
}

void Boomerang::Update() {

	if (goTimer_.IsActive()) {
		transform_.translate += directionToEnemy_ * (speed_ * goTimer_.GetReverseProgress()) * deltaTime_;
		goTimer_.Update();

		if (goTimer_.IsFinished()) {
			backTimer_.Start(2.0f, false);
		}
	} else {
		transform_.translate += -directionToEnemy_ * (speed_ * backTimer_.GetProgress()) * deltaTime_;
	}

	transform_.SetAllScale(scaleRate_);

	trailParticle_->SetEmitterPosition(model_->GetVertexWorldPosition(145));
	trail2Particle_->SetEmitterPosition(model_->GetVertexWorldPosition(202));
	trailParticle_->Update();
	trail2Particle_->Update();

	sphereCollision_.center = transform_.translate;
	transform_.rotate.y += 4.00f * deltaTime_;
	sphereCollision_.radius = scaleRate_;

	MyDebugLine::AddShape(sphereCollision_);

	backTimer_.Update();
	lifeTimer_.Update();

	if (lifeTimer_.GetProgress() >= 0.9f) {
		if (!scaleTimer_.IsActive()) {
			scaleTimer_.Start(lifeTime_ / 10.0f, false);
		}
	}

	if (lifeTimer_.IsActive()) {
		lifeTimer_.Update();
	}

	if (lifeTimer_.IsFinished()) {
		Dead();
	}
}

void Boomerang::Draw(Camera camera) {
	if (isAlive_) {
		model_->Draw(camera, transform_);
		trailParticle_->Draw(camera);
		trail2Particle_->Draw(camera);
	}
}

void Boomerang::DrawImGui() {

}