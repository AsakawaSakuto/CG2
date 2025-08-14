#include"PlayerBullet.h"

void PlayerBullet::Initialize(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    model_->Initialize(dxCommon_, "resources/object3d/player/playerBullet.obj");
    //model_->SetUseLight(false);

    smork_->Initialize(dxCommon_, "resources/image/particle/smork.png", 1);
    smork_->SetUseEmitter(true);

    smorkEmitter_.count = 1;
    smorkEmitter_.radius = 0.01f;
    smorkEmitter_.frequency = 0.01f;
    smorkEmitter_.isMove = true;

    smorkRange_.minScale = { 0.0f,0.0f,0.0f };
    smorkRange_.maxScale = { 0.5f,0.5f,0.0f };
    smorkRange_.minVelocity = { -0.1f,-0.1f,-0.2f };
    smorkRange_.maxVelocity = { 0.1f,0.1f,0.2f };
    smorkRange_.minColor = { 1.0f,1.0f,1.0f };
    smorkRange_.maxColor = { 1.0f,1.0f,1.0f };
    smorkRange_.minLifeTime = 0.1f;
    smorkRange_.maxLifeTime = 0.1f;

    smork_->SetEmitterValue(smorkEmitter_);
    smork_->SetEmitterRange(smorkRange_);
}

void PlayerBullet::Update(Camera* camera) {

    lifeTimer_ += deltaTime_;
    if (lifeTimer_>=lifeTime_) {
        isAlive_ = false;
    }

    if (isAlive_) {
        Vector3 scale = model_->GetScale();
        scale.x -= 0.5f * deltaTime_;
        scale.y -= 0.5f * deltaTime_;
        scale.z -= 0.5f * deltaTime_;
        smorkRange_.maxScale = scale;
        model_->SetScale(scale);
    } else {
        model_->SetScale({ 1.0f,1.0f,1.0f });
    }

    if (isAlive_) {
        speed_ += 100.0f * deltaTime_;
    } else {
        speed_ = 0.0f;
    }
    speed_ = std::clamp(speed_, 0.0f, 200.0f);

    Vector3 translate = model_->GetTranslate();
    translate += velocity_ * speed_ * deltaTime_;
    model_->SetTranslate(translate);

    Vector3 rotateValue = { 0.0f,0.0f,2.0f };
    Vector3 rotate = model_->GetRotate();
    rotate += rotateValue * deltaTime_;
    model_->SetRotate(rotate);

    smork_->SetEmitterValue(smorkEmitter_);
    smork_->SetEmitterRange(smorkRange_);
    smork_->SetEmitterPosition(model_->GetTranslate());

    model_->Update(*camera);
    smork_->Update(*camera);
}

void PlayerBullet::Draw() {
    model_->Draw();
    if (isAlive_) {
        smork_->Draw();
    }
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