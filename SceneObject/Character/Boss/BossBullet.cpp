#include"BossBullet.h"

void BossBullet::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/object3d/boss/bullet/bullet.obj");
    heal_->Initialize(dxCommon_, "resources/object3d/boss/bullet/enemyHealBulletobj.obj");

    isMove_ = false;
    isAlive_ = false;
    lifeTimer_ = 0.0f;
    scale_ = { 0.0f,0.0f,0.0f };

    bState_ = HEAL;
    randNum_ = 0;
}

void BossBullet::Update(Camera* camera) {
    if (isAlive_) {
        scale_.x += deltaTime_;
        scale_.y += deltaTime_;
        scale_.z += deltaTime_;
        scale_.x = std::clamp(scale_.x, 0.0f, 1.0f);
        scale_.y = std::clamp(scale_.y, 0.0f, 1.0f);
        scale_.z = std::clamp(scale_.z, 0.0f, 1.0f);
        if (scale_.x >= 1.0f) {
            isMove_ = true;
        }
    } else {
        scale_ = { 0.0f,0.0f,0.0f };
    }

    if (isMove_) {
        Vector3 translate = model_->GetTranslate();
        translate += velocity_ * speed_ * deltaTime_ * -1.0f;
        model_->SetTranslate(translate);
        lifeTimer_ += deltaTime_;
        if (lifeTimer_ >= lifeTime_) {
            isMove_ = false;
            isAlive_ = false;
            lifeTimer_ = 0.0f;
        }

        Vector3 translate2 = heal_->GetTranslate();
        translate2 += velocity_ * speed_ * deltaTime_ * -1.0f;
        heal_->SetTranslate(translate2);
        lifeTimer_ += deltaTime_;
        if (lifeTimer_ >= lifeTime_) {
            isMove_ = false;
            isAlive_ = false;
            lifeTimer_ = 0.0f;
        }
    }

    model_->SetScale(scale_);
    model_->Update(*camera);

    heal_->SetScale(scale_);
    heal_->Update(*camera);
}

void BossBullet::Draw() {
    if (bState_ == DAMAGE){
        model_->Draw();
    } else if (bState_ == HEAL) {
        heal_->Draw();
    }
}

void BossBullet::Spawn(Vector3 translate, Vector3 velocity) {
    model_->SetTranslate(translate);
    scale_ = { 0.0f,0.0f,0.0f };
    model_->SetScale(scale_);

    heal_->SetTranslate(translate);
    heal_->SetScale(scale_);

    randNum_ = rand_.Int(1, 100);
    if (randNum_<=15) {
        bState_ = HEAL;
    } else {
        bState_ = DAMAGE;
    }

    isAlive_ = true;
    lifeTimer_ = 0.0f;
    velocity_ = velocity;

    Vector3 rotate = {};
    rotate.y = std::atan2(velocity_.x, velocity_.z);
    float horizontalLength = std::sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
    rotate.x = std::atan2(-velocity_.y, horizontalLength);
    model_->SetRotate(rotate);
    heal_->SetRotate(rotate);
}

void BossBullet::Hit() {
    isMove_ = false;
    isAlive_ = false;
    lifeTimer_ = 0.0f;
}

Vector3 BossBullet::GetPos() {
    Vector3 pos;

    if (bState_ == DAMAGE) {
        pos = model_->GetWorldPosition();
    } else if (bState_ == HEAL) {
        pos = heal_->GetWorldPosition();
    }

    return pos;
}