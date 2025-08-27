#include"Boss.h"

void Boss::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	body_->Initialize(dxCommon_, "resources/object3d/boss/body.obj");
	body_->SetTranslate({ 0.0f,50.0f,50.0f });
	halo_->Initialize(dxCommon_, "resources/object3d/boss/halo.obj");
	ringL_->Initialize(dxCommon_, "resources/object3d/boss/ringL.obj");
	ringR_->Initialize(dxCommon_, "resources/object3d/boss/ringR.obj");

	hpUI_->Initialize(dxCommon_, "resources/image/UI/bossHpBar.png", { 1280.0f, 720.0f });
	hpUI_->SetPosition({ 640.0f,373.0f });
	hpUI2_->Initialize(dxCommon_, "resources/image/UI/bossHpBar2.png", { 1280.0f, 720.0f });
	hpUI2_->SetPosition({ 640.0f,373.0f });
	
	hpBar_->Initialize(dxCommon_, "resources/image/2.png", { 1.0f, 1.0f });
	hpBar_->SetScale({ 390.0f,15.0f });
	hpBar_->SetPosition({ 330.0f,43.0f });

	bullet_->Initialize(dxCommon_);
	arm_->Initialize(dxCommon_);

	isStart_ = false;

	haloRy_ = 0.0f;
	haloSpinSpeed_ = 3.0f;
	bulletShotTimer_ = 0.0f;
	haloIsShot_ = false;

	time_ = 0.0f;
	startPosition_ = {};

	life_ = 1.0f;

	InitParticle();

	tuirakuSE_->Initialize("resources/sound/SE/tuiraku.mp3");
	foolSE_->Initialize("resources/sound/SE/fool.mp3");
	bSpawnSE_->Initialize("resources/sound/SE/bossBulletSpawn.mp3");
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
		const float ampX = 20.0f; // X方向 ±n
		const float ampY = 8.0f;  // Y方向 ±n

		const float rX = 0.4f; // X方向 ±n
		const float rY = 0.8f;  // Y方向 ±n

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

		Vector3 velocity = body_->GetWorldPosition() - playerPos_;
		Vector3 rotate = body_->GetRotate();
		rotate.y = std::atan2(velocity.x, velocity.z);
		float horizontalLength = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
		rotate.x = std::atan2(-velocity.y, horizontalLength);
		body_->SetRotate(rotate);
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

	hpUI_->Update();
	hpUI2_->Update();

	life_ = std::clamp(life_, 0.0f, 390.0f);
	float tempTx = 135.0f + (life_ / 2.0f);
	hpBar_->SetScale({life_,15.0f });
	hpBar_->SetPosition({ tempTx,43.0f });
	hpBar_->Update();

	tuirakuSE_->Update();
	foolSE_->Update();
	bSpawnSE_->Update();
}

void Boss::DieUpdate(Camera* camera) {

	Vector3 bR = body_->GetRotate();
	Vector3 bT = body_->GetTranslate();

	if (bT.y >= -14.99f && bT.y <= -14.85f) {
		tuirakuSE_->PlayAudio();
		foolSE_->Reset();
	}

	if (bT.y <= -15.0f) {
		bT.y = -15.0f;
	} else {
		bT.y -= deltaTime_ * dieFoolSpeed_;
		bT.z += deltaTime_ * 10.0f;

		bR.x += deltaTime_ * dieRotateSpeed_;
		bR.y -= deltaTime_ * dieRotateSpeed_;
		bR.z += deltaTime_ * dieRotateSpeed_;
	}

	body_->SetTranslate(bT);
	body_->SetRotate(bR);

	body_->Update(*camera);

	arm_->DieUpdate(camera);

	halo_->SetTranslate(body_->GetTranslate());
	halo_->Update(*camera);
	ringL_->SetTranslate(body_->GetTranslate());
	ringL_->Update(*camera);
	ringR_->SetTranslate(body_->GetTranslate());
	ringR_->Update(*camera);

	leftFire_->UseEmitter(false);
	rightFire_->UseEmitter(false);

	leftFire_->SetEmitterPosition(body_->GetTranslate());
	leftFire_->SetOffSet({ -2.0f,-3.0f,0.0f });
	leftFire_->Update(*camera);
	rightFire_->SetEmitterPosition(body_->GetTranslate());
	rightFire_->SetOffSet({ 2.0f,-3.0f,0.0f });
	rightFire_->Update(*camera);

	dieSmork_->SetEmitterPosition(body_->GetTranslate());
	dieSmork_->Update(*camera);

	dieFire_->SetEmitterPosition(body_->GetTranslate());
	dieFire_->Update(*camera);
	if (bT.y >= -14.0f) {
		dieFire_->UseEmitter(false);
		dieFire_->SetEmit(false);
	} else {
		dieSmork_->UseEmitter(false);
		dieSmork_->SetEmit(false);

		dieFire_->UseEmitter(true);
	}

	tuirakuSE_->Update();
	foolSE_->Update();
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

	if (IsDie()) {
		dieSmork_->Draw();
		dieFire_->Draw();
	}

	hpUI2_->Draw();
	hpBar_->Draw();
	hpUI_->Draw();
}

void Boss::DrawImGui() {
	body_->DrawImGui("b");
	//arm_->DrawImGui();
	//hpBar_->DrawImGui("bar");
	//hpUI_->DrawImGui("1");
	//hpUI2_->DrawImGui("2");
	//ImGui::Begin("b");
	//ImGui::DragFloat("foolS", &dieFoolSpeed_, 0.1f);
	//ImGui::DragFloat("RotateS", &dieRotateSpeed_, 0.1f);
	//ImGui::End();
	//body_->DrawImGui("body");
	//dieSmork_->DrawImGui("dieSmork");
	//dieFire_->DrawImGui("dieFire");
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

	dieSmork_->Initialize(dxCommon_, "resources/image/particle/Smork.png", 2);
	dieSmork_->UseEmitter(true);

	dieSmorkEmitter_.count = 10;
	dieSmorkEmitter_.isMove = true;
	dieSmorkEmitter_.radius = 0.01f;
	dieSmorkEmitter_.spawnTime = 0.01f;
	dieSmork_->SetEmitterValue(dieSmorkEmitter_);

	dieSmorkRange_.minScale = { 0.1f,0.1f,0.0f };
	dieSmorkRange_.maxScale = { 1.25f,1.25f,0.0f };
	dieSmorkRange_.minVelocity = { -0.2f,0.25f,-0.1f };
	dieSmorkRange_.maxVelocity = { 0.2f,1.0f,0.1f };
	dieSmorkRange_.minColor = { 1.0f,1.0f,1.0f };
	dieSmorkRange_.maxColor = { 1.0f,1.0f,1.0f };
	dieSmorkRange_.minLifeTime = 0.1f;
	dieSmorkRange_.maxLifeTime = 1.0f;
	dieSmork_->SetEmitterRange(dieSmorkRange_);

	dieFire_->Initialize(dxCommon_, "resources/image/particle/circle.png", 50);

	dieFireEmitter_.count = 100;
	dieFireEmitter_.isMove = true;
	dieFireEmitter_.radius = 0.01f;
	dieFireEmitter_.spawnTime = 0.01f;
	dieFire_->SetEmitterValue(dieFireEmitter_);

	dieFireRange_.minScale = { 0.1f,0.1f,0.0f };
	dieFireRange_.maxScale = { 5.0f,5.0f,0.0f };
	dieFireRange_.minVelocity = { -0.75f,0.0f,-0.1f };
	dieFireRange_.maxVelocity = { 0.75f,5.0f,0.1f };
	dieFireRange_.minColor = { 0.75f,0.1f,0.0f };
	dieFireRange_.maxColor = { 1.0f,0.3f,0.0f };
	dieFireRange_.minLifeTime = 0.1f;
	dieFireRange_.maxLifeTime = 1.0f;
	dieFire_->SetEmitterRange(dieFireRange_);
}

void Boss::UpdateHalo() {
	Vector3 velo = halo_->GetWorldPosition() - playerPos_ += {0.0f, 7.5f, 0.0f};
	velo = velo.Normalize();
	if (!bullet_->GetIsMove()) {
		bullet_->SetVelocity(velo); 
		bullet_->SetTranslate(halo_->GetTranslate() += {0.0f, 7.5f, 0.0f});
	}

	if (haloIsShot_) {
		haloSpinSpeed_ = 20.0f;
		bulletShotTimer_ += deltaTime_;
		if (bulletShotTimer_ >= 1.0f) {
			bullet_->Spawn(halo_->GetTranslate() += {0.0f, 7.5f,0.0f}, velo);
			bulletShotTimer_ = 0.0f;
			haloIsShot_ = false;

			bSpawnSE_->PlayAudio();
			bSpawnSE_->SetVolume(0.2f);
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

void Boss::CloseSound() {
	tuirakuSE_->Reset();
	foolSE_->Reset();
	bSpawnSE_->Reset();
	bullet_->CloseSound();
	arm_->CloseSound();
}