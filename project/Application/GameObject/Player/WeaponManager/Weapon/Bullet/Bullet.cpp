#include "bullet.h"

void Bullet::Initialize(AppContext* ctx) {
    ctx = ctx;
    model_->Initialize(&ctx->dxCommon, "cube.obj");
}

void Bullet::Update() {

    model_->Update();
}

void Bullet::Draw(Camera camera) {

    model_->Draw(camera);
}

void Bullet::DrawImGui() {

}