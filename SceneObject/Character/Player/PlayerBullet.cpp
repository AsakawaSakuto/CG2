#include"PlayerBullet.h"

void PlayerBullet::Initialize(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    model_->Initialize(dxCommon_, "resources/object3d/player/playerBullet.obj");
}

void PlayerBullet::Update(Camera* camera) {

    lifeTimer_ += deltaTime_;
    if (lifeTimer_>=lifeTime_) {
        isAlive_ = false;
    }

    Vector3 translate = model_->GetTranslate();
    translate.z += 20.0f * deltaTime_;
    model_->SetTranslate(translate);

    model_->Update(*camera);
}

void PlayerBullet::Draw() {
    model_->Draw();
}

void PlayerBullet::DrawImGui() {
    model_->DrawImGui("player");
}

void PlayerBullet::Spawn(Vector3 translate) {
    model_->SetTranslate(translate);
    isAlive_ = true;
    lifeTimer_ = 0.0f;
}