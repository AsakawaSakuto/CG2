#include"BossArm.h"

void BossArm::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	armL_->Initialize(dxCommon_, "resources/object3d/boss/armL.obj");
	armL_->SetTranslate({ -8.0f,0.0f,50.0f });
	armR_->Initialize(dxCommon_, "resources/object3d/boss/armR.obj");
	armR_->SetTranslate({ 8.0f,0.0f,50.0f });

	leftFire_->Initialize(dxCommon_, "resources/image/particle/fire.png", 2);
	rightFire_->Initialize(dxCommon_, "resources/image/particle/fire.png", 2);
	InitParticle();

	bossPos_ = {};
	armPosL_ = { -8.0f, 0.0f, 50.0f };
	armPosR_ = { 8.0f,0.0f,50.0f };

	isAction = false;
	actionTimer_ = 0.0f;
	isChange = false;
	changeTimer_ = 0.0f;

	armShotSE_->Initialize("resources/sound/SE/armShot.mp3");
	armFireSE_->Initialize("resources/sound/SE/armFire.mp3");
}

void BossArm::Update(Camera* camera) {
	if (isAction) {
		leftFire_->UseEmitter(true);
		rightFire_->UseEmitter(true);

		armPosL_ = armL_->GetTranslate();
		armPosL_.x -= startSpeed_ * deltaTime_;

		armPosR_ = armR_->GetTranslate();
		armPosR_.x += startSpeed_ * deltaTime_;

		changeTimer_ += deltaTime_;
		if (changeTimer_ >= changeTime_ && !isChange) {
			isChange = true;
			changeTimer_ = 0.0f;
			armPosL_ = { 15.0f,0.0f,0.0f };
			armPosR_ = { -15.0f,8.0f,0.0f };

			armFireSE_->PlayAudio();
		} else if (changeTimer_ >= changeTime_ && isChange) {
			isChange = false;
			isAction = false;
			changeTimer_ = 0.0f;
			armPosL_ = { -35.0f,0.0,50.0f };
			armPosR_ = { 35.0f,0.0,50.0f };
		}

		armR_->SetTranslate(armPosR_);
		armL_->SetTranslate(armPosL_);

	} else {
		leftFire_->UseEmitter(false);
		leftFire_->SetEmit(false);
		rightFire_->UseEmitter(false);
		rightFire_->SetEmit(false);

		armPosL_ = armL_->GetTranslate();
		armPosL_.x += startSpeed_ * deltaTime_;
		armPosL_.x = std::clamp(armPosL_.x, -100.0f, bossPos_.x - 8.0f);

		armPosR_ = armR_->GetTranslate();
		armPosR_.x -= startSpeed_ * deltaTime_;
		armPosR_.x = std::clamp(armPosR_.x, bossPos_.x + 8.0f, 100.0f);

		armR_->SetTranslate({ armPosR_.x ,bossPos_.y,bossPos_.z });
		armL_->SetTranslate({ armPosL_.x ,bossPos_.y,bossPos_.z });

		actionTimer_ += deltaTime_;
		if (actionTimer_ >= actionTime_) {
			actionTimer_ = 0.0f;
			isAction = true;

			armShotSE_->PlayAudio();
		}
	}

	leftFire_->SetEmitterPosition(armL_->GetWorldPosition());
	leftFire_->SetOffSet({ 3.25f,0.0f,0.0f });

	rightFire_->SetEmitterPosition(armR_->GetWorldPosition());
	rightFire_->SetOffSet({ -3.25f,0.0f,0.0f });

	leftFire_->Update(*camera);
	rightFire_->Update(*camera);

	armL_->Update(*camera);
	armR_->Update(*camera);

	armShotSE_->Update();
	armFireSE_->Update();
}

void BossArm::DieUpdate(Camera* camera) {

	Vector3 alT = armL_->GetTranslate();

	if (alT.y <= -15.0f) {
		alT.y = -15.0f;
	} else {
		alT.y -= 7.5f * deltaTime_;
		alT.z += 10.0f * deltaTime_;
	}

	armL_->SetTranslate(alT);

	Vector3 arT = armR_->GetTranslate();
	
	if (arT.y <= -15.0f) {
		arT.y = -15.0f;
	} else {
		arT.y -= 7.5f * deltaTime_;
		arT.z += 10.0f * deltaTime_;
	}

	armR_->SetTranslate(arT);

	leftFire_->SetEmitterPosition(armL_->GetWorldPosition());
	leftFire_->SetOffSet({ 3.25f,0.0f,0.0f });

	rightFire_->SetEmitterPosition(armR_->GetWorldPosition());
	rightFire_->SetOffSet({ -3.25f,0.0f,0.0f });

	leftFire_->Update(*camera);
	rightFire_->Update(*camera);

	armL_->Update(*camera);
	armR_->Update(*camera);
}

void BossArm::Draw() {
	leftFire_->Draw();
	rightFire_->Draw();

	armL_->Draw();
	armR_->Draw();
}

void BossArm::DrawImGui() {
	//armL_->DrawImGui("aL");
	//armR_->DrawImGui("aR");
	//leftFire_->DrawImGui("FL");
	//rightFire_->DrawImGui("FR");
}

void BossArm::InitParticle() {
	fireEmitterL_.count = 5;
	fireEmitterL_.isMove = true;
	fireEmitterL_.radius = 0.01f;
	fireEmitterL_.spawnTime = 0.01f;
	leftFire_->SetEmitterValue(fireEmitterL_);

	fireRangeL_.minScale = { 0.1f,0.1f,0.0f };
	fireRangeL_.maxScale = { 1.25f,1.25f,0.0f };
	fireRangeL_.minVelocity = { 0.0f,-0.05f,-0.05f };
	fireRangeL_.maxVelocity = { 0.1f,0.05f,0.05f };
	fireRangeL_.minColor = { 0.5f,0.0f,0.0f };
	fireRangeL_.maxColor = { 1.0f,0.25f,0.0f };
	fireRangeL_.minLifeTime = 0.1f;
	fireRangeL_.maxLifeTime = 0.5f;
	leftFire_->SetEmitterRange(fireRangeL_);

	fireEmitterR_.count = 5;
	fireEmitterR_.isMove = true;
	fireEmitterR_.radius = 0.01f;
	fireEmitterR_.spawnTime = 0.01f;
	rightFire_->SetEmitterValue(fireEmitterR_);

	fireRangeR_.minScale = { 0.1f,0.1f,0.0f };
	fireRangeR_.maxScale = { 1.25f,1.25f,0.0f };
	fireRangeR_.minVelocity = { 0.0f,-0.05f,-0.05f };
	fireRangeR_.maxVelocity = { 0.1f,0.05f,0.05f };
	fireRangeR_.minColor = { 0.5f,0.0f,0.0f };
	fireRangeR_.maxColor = { 1.0f,0.25f,0.0f };
	fireRangeR_.minLifeTime = 0.1f;
	fireRangeR_.maxLifeTime = 0.5f;
	rightFire_->SetEmitterRange(fireRangeR_);
}