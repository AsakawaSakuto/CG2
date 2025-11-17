#include "bullet.h"

void Bullet::Initialize(AppContext* ctx) {
    ctx = ctx;
    model_->Initialize(&ctx->dxCommon, "cube.obj");

	transform_.scale = { 0.25f,0.25f,0.25f };
}

void Bullet::Update() {

    transform_.translate.y += 1.0f * deltaTime_;

    model_->Update();
}

void Bullet::Draw(Camera camera) {

    model_->Draw(camera, transform_);
}

void Bullet::DrawImGui() {

}