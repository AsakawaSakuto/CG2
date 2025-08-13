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
   
    translate += velocity_ * deltaTime_;

    model_->SetTranslate(translate);

    model_->Update(*camera);
}

void PlayerBullet::Draw() {
    model_->Draw();
}

void PlayerBullet::DrawImGui() {
    model_->DrawImGui("player");
}

void PlayerBullet::Spawn(Vector3 translate, Vector3 velocity) {
    model_->SetTranslate(translate);
    isAlive_ = true;
    lifeTimer_ = 0.0f;
    velocity_ = velocity;

    Vector3 rotate = {};
    rotate.y = std::atan2(velocity_.x, velocity_.z);
    float horizontalLength = std::sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
    rotate.x = std::atan2(-velocity_.y, horizontalLength);
    model_->SetRotate(rotate);
}