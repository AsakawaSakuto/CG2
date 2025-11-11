#include "Enemy.h"

void Enemy::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.translate = { 0.f,0.f,0.f };

	model_->Initialize(&ctx_->dxCommon, "enemy/enemy.obj");
	model_->SetUpdateFrustumCulling(true);
}

void Enemy::Update() {

	transform_.translate.y += 5.0f * deltaTime_;

	model_->SetTransform(transform_);
	model_->Update();
}

void Enemy::Draw(Camera camera) {
	model_->Draw(camera);
}

void Enemy::DrawImGui() {

}