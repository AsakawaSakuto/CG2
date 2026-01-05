#include "Dice.h"

void Dice::Initialize() {

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.SetAllScale(1.0f);

	model_->Initialize("weapon/dice/dice.obj");
	lifeTimer_.Start(lifeTime_, false);
}

void Dice::Update() {

	transform_.translate += directionToEnemy_ * speed_ * deltaTime_;
	Vector3 r = { 2.0f, 3.0f, 1.0f };
	transform_.rotate += r * deltaTime_;

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 1.0f;

	MyDebugLine::AddShape(sphereCollision_);

	lifeTimer_.Update();

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
}

void Dice::Draw(Camera camera) {
	if (isAlive_) {
		model_->Draw(camera, transform_);
	}
}

void Dice::DrawImGui() {

}