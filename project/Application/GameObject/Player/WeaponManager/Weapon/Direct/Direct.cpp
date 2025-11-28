#include "Direct.h"

void Direct::Initialize(AppContext* ctx) {
	ctx_ = ctx;

	transform_.translate = { 0.0f,0.0f,0.0f };

	particle_->Initialize(&ctx_->dxCommon);
	particle2_->Initialize(&ctx_->dxCommon);

	lifeTimer_.Start(10.0f, false);
}

void Direct::Update() {

	// Sphere collider update
	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.5f;

	transform_.rotate.y += 1.0f * deltaTime_;

	particle_->SetEmitterPosition(transform_.translate);
	particle_->Update();

	particle2_->SetEmitterPosition(transform_.translate);
	particle2_->Update();

	if (lifeTimer_.GetProgress() >= 0.9f) {
		particle_->Stop();
		particle2_->Stop();
	}

	if (lifeTimer_.IsFinished()) {
		Dead();
	}

	lifeTimer_.Update();
}

void Direct::Draw(Camera camera) {
	if (isAlive_) {
		particle_->Draw(camera);
		particle2_->Draw(camera);
	}
}

void Direct::DrawImGui() {

}