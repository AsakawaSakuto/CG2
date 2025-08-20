#include "PlayerBeam.h"

void PlayerBeam::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/object3d/player/playerBeam.obj");
	model_->SetTranslate({ 0.0f, 0.0f, -100.0f });

	pBeam_->Initialize(dxCommon_, "resources/image/particle/circle.png", 2);
	pBeam_->SetUseEmitter(true);

	beamEmitter_.count = 100;
	beamEmitter_.isMove = true;
	beamEmitter_.radius = 1.5f;

	beamRange_.minScale = { 0.1f,0.1f,0.1f };
	beamRange_.maxScale = { 1.0f,1.0f,1.0f };
	beamRange_.minVelocity = { -0.1f,-0.1f,-0.1f };
	beamRange_.maxVelocity = { 0.1f,0.1f,0.1f };
	beamRange_.minColor = { 0.0f,0.0f,0.2f };
	beamRange_.maxColor = { 0.0f,0.5f,0.8f };
	beamRange_.minLifeTime = 0.1f;
	beamRange_.maxLifeTime = 0.5f;

	pBeam_->SetEmitterValue(beamEmitter_);
	pBeam_->SetEmitterRange(beamRange_);
}

void PlayerBeam::Update(Camera* camera) {
	if (isAlive_) {
		timer_ += deltaTime_;
		if (timer_ >= time_) {
			timer_ = 0.0f;
			isAlive_ = false;
		}

		Vector3 translate = model_->GetTranslate();
		translate += velocity_ * speed_ * deltaTime_;
		model_->SetTranslate(translate);

		model_->Update(*camera);

	} else {
		if (isDelete_) {
			model_->SetTranslate({ 0.0f, 0.0f, -100.0f });
		} else {
			deleteTimer_ += deltaTime_;
			if (deleteTimer_ >= deleteTime_) {
				deleteTimer_ = 0.0f;
				isDelete_ = true;
			}
		}
	}

	pBeam_->SetEmitterPosition(model_->GetTranslate());
	pBeam_->Update(*camera);
}

void PlayerBeam::Draw() {
	if (isAlive_) {
		model_->Draw();
	}
	pBeam_->Draw();
}

void PlayerBeam::DrawImGui() {
	//pBeam_->DrawImGui("b");
}

void PlayerBeam::Spawn(Vector3 translate, Vector3 velocity) {
    model_->SetTranslate(translate);
    isAlive_ = true;
	isDelete_ = false;
    velocity_ = velocity;
	timer_ = 0.0f;
	deleteTimer_ = 0.0f;

	beamRange_.minScale = { 0.1f,0.1f,0.1f };
	beamRange_.maxScale = { 1.0f,1.0f,1.0f };
	beamRange_.minVelocity = { -0.1f,-0.1f,-0.1f };
	beamRange_.maxVelocity = { 0.1f,0.1f,0.1f };
	beamRange_.minColor = { 0.0f,0.0f,0.2f };
	beamRange_.maxColor = { 0.0f,0.5f,0.8f };
	beamRange_.minLifeTime = 0.1f;
	beamRange_.maxLifeTime = 0.5f;
	pBeam_->SetEmitterRange(beamRange_);
}

void PlayerBeam::Hit() {
	beamRange_.minScale = { 0.1f,0.1f,0.1f };
	beamRange_.maxScale = { 1.0f,1.0f,1.0f };
	beamRange_.minVelocity = { -1.0f,-1.0f,-1.0f };
	beamRange_.maxVelocity = { 1.0f,1.0f,1.0f };
	beamRange_.minColor = { 0.0f,0.0f,0.2f };
	beamRange_.maxColor = { 0.0f,0.5f,0.8f };
	beamRange_.minLifeTime = 0.1f;
	beamRange_.maxLifeTime = 0.5f;
	pBeam_->SetEmitterRange(beamRange_);

	isAlive_ = false;
}