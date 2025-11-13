#include "bullet.h"

void Bullet::Initialize(AppContext* ctx) {
    ctx = ctx;
    model_->Initialize(&ctx->dxCommon, "cube.obj");

    transform_.scale = { 1.0f,1.0f,1.0f };
}

void Bullet::Update() {

    transform_.translate.y += 1.0f * deltaTime_;

	model_->SetTransform(transform_);
    model_->Update();
}

void Bullet::Draw(Camera camera) {

    model_->Draw(camera);
}

void Bullet::DrawImGui() {

}