#include "Area.h"

void Area::Initialize() {

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.scale = { 3.2f,3.2f,3.2f };

	particle_->Initialize();
	particle2_->Initialize();

	model_->Initialize("weapon/area.obj");
	model_->SetTexture("resources/model/weapon/magic.png");
	model_->SetColor3({ 0.529f, 0.808f, 0.922f });

	lifeTimer_.Start(7.5f, false);
}

void Area::Update() {

	// Sphere collider update
	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.5f;

	transform_.rotate.y += 1.0f * deltaTime_;

	particle_->SetEmitterPosition(transform_.translate);
	particle_->Update();

	particle2_->SetEmitterPosition(transform_.translate);
	particle2_->Update();

	model_->Update();

	if (lifeTimer_.GetProgress() >= 0.9f) {
		particle_->Stop();
		particle2_->Stop();
		if (!scaleTimer_.IsActive()) {
			scaleTimer_.Start(0.75f, false);
		}
	}

	if (lifeTimer_.IsFinished()) {
		Dead();
	}

	if (scaleTimer_.IsActive()) {
		transform_.scale = Easing::Lerp(Vector3{ 3.2f,3.2f,3.2f }, Vector3{ 0.0f,0.0f,0.0f }, scaleTimer_.GetProgress());
	}

	lifeTimer_.Update();
	scaleTimer_.Update();
}

void Area::Draw(Camera camera) {
	if (isAlive_) {
		model_->Draw(camera, transform_);
		particle_->Draw(camera);
		particle2_->Draw(camera);
	}
}

void Area::DrawImGui() {

}