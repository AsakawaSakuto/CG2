#include "Area.h"

void Area::Initialize() {

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.SetAllScale(1.0f);

	model_->Initialize("weapon/Area/Area.obj");
	model_->SetColor3({ 0.094f, 0.627f, 0.576f });

	areaParticle_->Initialize("area");
}

void Area::Update() {

	transform_.rotate.y += 6.0f * deltaTime_;

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 3.0f;

	areaParticle_->SetEmitterPosition(transform_.translate);
	areaParticle_->Update();
	MyDebugLine::AddShape(sphereCollision_);
}

void Area::Draw(Camera camera) {
	if (isAlive_) {
		model_->Draw(camera, transform_);
		areaParticle_->Draw(camera);
	}
}

void Area::DrawImGui() {

}