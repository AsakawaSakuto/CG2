#include "Gun.h"

void Gun::Initialize() {

	// パーティクルは後からSetSharedParticle()で設定される
	transform_.translate = { 0.0f,0.0f,0.0f };

	lifeTimer_.Start(10.0f, false);

	model_ = make_unique<Model>();
	model_->Initialize("weapon/gun/gun.obj");

	// 初回のみリソース確保、2回目以降はリセットのみ
	// Particlesクラス内部で isInitialized_ フラグを使用して判定
	particle_->Initialize("Gun");
}

void Gun::Update() {

	transform_.translate += directionToEnemy_ * speed_ * deltaTime_;

	// 飛んでいく方向にモデルを向ける
	if (directionToEnemy_.LengthSq() > 0.0f) {
		// Y軸周りの回転（Yaw）- XZ平面での方向
		transform_.rotate.y = std::atan2(directionToEnemy_.x, directionToEnemy_.z);
		
		// X軸周りの回転（Pitch）- 上下方向
		float horizontalLength = std::sqrt(directionToEnemy_.x * directionToEnemy_.x + directionToEnemy_.z * directionToEnemy_.z);
		transform_.rotate.x = -std::atan2(directionToEnemy_.y, horizontalLength);
	}

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 1.0f;

	MyDebugLine::AddShape(sphereCollision_);

	particle_->SetEmitterPosition(transform_.translate);
	particle_->Update();

	lifeTimer_.Update();

	if (lifeTimer_.GetProgress() >= 0.9f) {
		particle_->Stop();
	}

	if (lifeTimer_.IsFinished()) {
		Dead();
	}
}

void Gun::Draw(Camera camera) {
	if (isAlive_ && lifeTimer_.GetDuration() >= 0.2f) {
		model_->Draw(camera, transform_);
		particle_->Draw(camera);
	}
}

void Gun::DrawImGui() {

}