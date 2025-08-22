#include"Boss.h"

void Boss::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	body_->Initialize(dxCommon_, "resources/object3d/boss/body.obj");
	body_->SetTranslate({ 0.0f,50.0f,50.0f });
	halo_->Initialize(dxCommon_, "resources/object3d/boss/halo.obj");
	ringL_->Initialize(dxCommon_, "resources/object3d/boss/ringL.obj");
	ringR_->Initialize(dxCommon_, "resources/object3d/boss/ringR.obj");

	left_->Initialize(dxCommon_, "resources/object3d/boss/armL.obj");
	left_->SetTranslate({ -10.0f,0.0f,50.0f });
	right_->Initialize(dxCommon_, "resources/object3d/boss/armR.obj");
	right_->SetTranslate({ 10.0f,0.0f,50.0f });

	isStart_ = false;

	InitParticle();
}

void Boss::Update(Camera* camera) {
	body_->Update(*camera);
	halo_->SetTranslate(body_->GetTranslate());
	halo_->Update(*camera);
	ringL_->SetTranslate(body_->GetTranslate());
	ringL_->Update(*camera);
	ringR_->SetTranslate(body_->GetTranslate());
	ringR_->Update(*camera);

	left_->Update(*camera);
	right_->Update(*camera);

	leftFire_->SetEmitterPosition(body_->GetTranslate());
	leftFire_->SetOffSet({ -2.0f,-3.0f,0.0f });
	leftFire_->Update(*camera);

	rightFire_->SetEmitterPosition(body_->GetTranslate());
	rightFire_->SetOffSet({ 2.0f,-3.0f,0.0f });
	rightFire_->Update(*camera);
}

void Boss::Draw() {
	body_->Draw();
	halo_->Draw();
	ringL_->Draw();
	ringR_->Draw();

	left_->Draw();
	right_->Draw();

	leftFire_->Draw();
	rightFire_->Draw();
}

void Boss::DrawImGui() {
	body_->DrawImGui("b");
	leftFire_->DrawImGui("lF");
	rightFire_->DrawImGui("RF");
}

void Boss::InitParticle() {
	leftFire_->Initialize(dxCommon_, "resources/image/particle/fire.png", 2);
	rightFire_->Initialize(dxCommon_, "resources/image/particle/fire.png", 2);

	fireEmitter_.count = 50;
	fireEmitter_.isMove = true;
	fireEmitter_.radius = 0.01f;
	fireEmitter_.spawnTime = 0.1f;
	leftFire_->SetEmitterValue(fireEmitter_);
	rightFire_->SetEmitterValue(fireEmitter_);

	fireRange_.minScale = { 0.1f,0.1f,0.0f };
	fireRange_.maxScale = { 1.25f,1.25f,0.0f };
	fireRange_.minVelocity = { 0.0f,-0.25f,-0.1f };
	fireRange_.maxVelocity = { 0.12f,0.0f,0.1f };
	fireRange_.minColor = { 0.5f,0.0f,0.0f };
	fireRange_.maxColor = { 1.0f,0.25f,0.0f };
	fireRange_.minLifeTime = 0.1f;
	fireRange_.maxLifeTime = 0.5f;
	rightFire_->SetEmitterRange(fireRange_);

	fireRange_.minVelocity = { -0.12f,-0.25f,-0.1f };
	fireRange_.maxVelocity = { 0.0f,0.0f,0.1f };
	leftFire_->SetEmitterRange(fireRange_);
}