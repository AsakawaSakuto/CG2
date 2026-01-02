#include "Toxic.h"

void Toxic::Initialize() {

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.SetAllScale(0.5f);

	model_->Initialize("weapon/toxic/toxic.obj");
	model_->SetColor3({ 1.0f, 0.0f, 1.0f });
	lifeTimer_.Start(lifeTime_, false);

	toxicParticle_->Initialize("toxic");
}

void Toxic::Update() {

	transform_.rotate.y += 6.0f * deltaTime_;

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 1.0f;

	MyDebugLine::AddShape(sphereCollision_);

	lifeTimer_.Update();

	if (lifeTimer_.GetProgress() >= 0.85f) {
		toxicParticle_->Stop();
	}

	if (lifeTimer_.GetProgress() >= 0.9f) {
		if (!scaleTimer_.IsActive()) {
			scaleTimer_.Start(lifeTime_ / 10.0f, false);
		}
	}

	if (scaleTimer_.IsActive()) {
		scaleTimer_.Update();
		transform_.scale = MyEasing::Lerp(Vector3{ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, scaleTimer_.GetProgress(), EaseType::Linear);
	}

	if (lifeTimer_.IsFinished()) {
		Dead();
	}

	toxicParticle_->SetEmitterPosition(transform_.translate);
	toxicParticle_->Update();
}

void Toxic::Draw(Camera camera) {
	if (isAlive_) {
		toxicParticle_->Draw(camera);
		model_->Draw(camera, transform_);
	}
}

void Toxic::DrawImGui() {

}