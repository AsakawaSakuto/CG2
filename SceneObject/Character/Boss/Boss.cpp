#include"Boss.h"

void Boss::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	body_->Initialize(dxCommon_, "resources/object3d/boss/body.obj");
	body_->SetTranslate({ 0.0f,50.0f,50.0f });
	halo_->Initialize(dxCommon_, "resources/object3d/boss/halo.obj");
	ringL_->Initialize(dxCommon_, "resources/object3d/boss/ringL.obj");
	ringR_->Initialize(dxCommon_, "resources/object3d/boss/ringR.obj");

	bullet_->Initialize(dxCommon_);
	arm_->Initialize(dxCommon_);

	isStart_ = false;

	haloRy_ = 0.0f;
	haloSpinSpeed_ = 3.0f;
	bulletShotTimer_ = 0.0f;
	haloIsShot_ = false;

	time_ = 0.0f;
	startPosition_ = {};

	InitParticle();
}

void Boss::Update(Camera* camera) {
	UpdateHalo();
	bullet_->Update(camera);

	arm_->SetBossPos(body_->GetWorldPosition());
	arm_->Update(camera);

	if (isStart_) {
		// 初回だけ現在のワールド座標を初期位置として記録
		if (time_ == 0.0f) {
			startPosition_ = body_->GetWorldPosition();
		}

		// 経過時間を更新
		time_ += deltaTime_;

		// 移動範囲（振幅）
		const float ampX = 20.0f; // X方向 ±20
		const float ampY = 8.0f;  // Y方向 ±10

		// 周期（秒）
		const float periodX = 20.0f; // Xはn秒で1往復
		const float periodY = 7.5f;  // Yはn秒で1往復

		// 角速度（2πで1周期）
		const float omegaX = 2.0f * 3.141592f / periodX;
		const float omegaY = 2.0f * 3.141592f / periodY;

		Vector3 translate = startPosition_;

		// Xはsin、Yはsinで中心から動かす
		translate.x = startPosition_.x + ampX * std::sin(omegaX * time_);
		translate.y = startPosition_.y + ampY * std::sin(omegaY * time_);
		translate.z = startPosition_.z;

		body_->SetTranslate(translate);
	}

	body_->Update(*camera);

	halo_->SetTranslate(body_->GetTranslate());
	halo_->Update(*camera);

	ringL_->SetTranslate(body_->GetTranslate());
	ringL_->Update(*camera);
	ringR_->SetTranslate(body_->GetTranslate());
	ringR_->Update(*camera);

	leftFire_->SetEmitterPosition(body_->GetTranslate());
	leftFire_->SetOffSet({ -2.0f,-3.0f,0.0f });
	leftFire_->Update(*camera);

	rightFire_->SetEmitterPosition(body_->GetTranslate());
	rightFire_->SetOffSet({ 2.0f,-3.0f,0.0f });
	rightFire_->Update(*camera);
}

void Boss::Draw() {
	bullet_->Draw();
	arm_->Draw();

	body_->Draw();
	halo_->Draw();
	ringL_->Draw();
	ringR_->Draw();

	leftFire_->Draw();
	rightFire_->Draw();
}

void Boss::DrawImGui() {
	body_->DrawImGui("b");
	arm_->DrawImGui();
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

void Boss::UpdateHalo() {
	Vector3 velo = playerPos_ - halo_->GetWorldPosition() -= {0.0f, 10.0f, 0.0f};
	if (!bullet_->GetIsMove()) {
		bullet_->SetVelocity(velo); 
		bullet_->SetTranslate(halo_->GetTranslate() += {0.0f, 10.0f, 0.0f});
	}

	if (haloIsShot_) {
		haloSpinSpeed_ = 10.0f;
		bulletShotTimer_ += deltaTime_;
		if (bulletShotTimer_ >= 1.0f) {
			bullet_->Spawn(halo_->GetTranslate() += {0.0f,10.0f,0.0f}, velo);
			bulletShotTimer_ = 0.0f;
			haloIsShot_ = false;
		}
	} else {
		haloSpinSpeed_ = 3.0f;
		bulletShotTimer_ += deltaTime_;
		if (bulletShotTimer_ >= bulletShotTime_) {
			bulletShotTimer_ = 0.0f;
			haloIsShot_ = true;
		}
	}
	haloRy_ += haloSpinSpeed_ * deltaTime_;
	halo_->SetRotate({ 0.0f,haloRy_,0.0f});
}