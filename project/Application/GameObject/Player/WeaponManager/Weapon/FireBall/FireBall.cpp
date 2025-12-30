#include "FireBall.h"

void FireBall::Initialize() {

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };
	transform_.scale = { 0.5f,0.5f,0.5f };

	lifeTimer_.Start(10.0f, false);

	// 初回のみリソース確保、2回目以降はリセットのみ
	// Particlesクラス内部で isInitialized_ フラグを使用して判定
	particle_->Initialize("fireBall");
	particle2_->Initialize("fireBall2");
}

void FireBall::Update() {

	transform_.translate += directionToEnemy_ * speed_ * deltaTime_;

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.5f;

	MyDebugLine::AddShape(sphereCollision_);

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

void FireBall::Draw(Camera camera) {
	if (isAlive_ && lifeTimer_.GetDuration() >= 0.2f) {
		particle_->Draw(camera);
		particle2_->Draw(camera);
	}
}

void FireBall::DrawImGui() {

}