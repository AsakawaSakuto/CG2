#include "Application/GameObject/Thorn/Particle/ThornParticle.h"

void ThornParticle::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	for (int i = 0; i < 10; i++) {
		pModel_[i] = std::make_unique<Model>();
		pModel_[i]->Initialize(dxCommon_, "Candy/Candy.obj");

		pTransform_[i].scale = { baseScale_,baseScale_,baseScale_ };
		pTransform_[i].rotate = { 0.0f,0.0f,0.0f };
		pTransform_[i].translate = { 0.0f,0.0f,0.0f };
	}
}

void ThornParticle::Update() {

	for (int i = 0; i < 10; i++) {
		pTransform_[i].translate += pVelocity_[i];

		pTransform_[i].rotate.x += pRotateVelocity_[i].x;
		pTransform_[i].rotate.y += pRotateVelocity_[i].y;
		pTransform_[i].rotate.z += pRotateVelocity_[i].z;

		if (lifeTimer_.IsActive()) {
			pTransform_[i].scale.x = baseScale_ * lifeTimer_.GetReverseProgress();
			pTransform_[i].scale.y = baseScale_ * lifeTimer_.GetReverseProgress();
			pTransform_[i].scale.z = baseScale_ * lifeTimer_.GetReverseProgress();
		}

		acceleration_ += gravity_ * deltaTime_;
		pVelocity_[i].y += acceleration_ * deltaTime_;

		pModel_[i]->SetColor({ 1.0,1.0f,1.0f,alphaTimer_.GetReverseProgress()});

		pModel_[i]->SetTransform(pTransform_[i]);
		pModel_[i]->Update();

		if (lifeTimer_.IsFinished()) {
			pIsAlive_[i] = false;
			pTransform_[i].scale = { 0.0f ,0.0f ,0.0f };
			lifeTimer_.Reset();
		}
	}

	lifeTimer_.Update();
	alphaTimer_.Update();
}

void ThornParticle::Draw(Camera useCamera) {
	for (int i = 0; i < 10; i++) {
		if (pIsAlive_[i]) {
			pModel_[i]->Draw(useCamera);
		}
	}
}

void ThornParticle::Play(Vector3 pos, uint32_t playNum) {
	Random rand;
	for (int i = 0; i < static_cast<int>(playNum); i++) {
		pVelocity_[i].x = rand.Float(-0.1f, 0.1f);
		pVelocity_[i].y = rand.Float(-0.1f, 0.1f);

		pRotateVelocity_[i].x = rand.Float(-0.0f, 0.0f);
		pRotateVelocity_[i].y = rand.Float(-0.0f, 0.0f);
		pRotateVelocity_[i].z = rand.Float(-0.25f, 0.25f);
		pVelocity_[i].z = 0.0f;

		pTransform_[i].translate = pos;
		pTransform_[i].scale = { baseScale_,baseScale_,baseScale_ };

		pIsAlive_[i] = true;

		acceleration_ = 0.0f;
	}

	lifeTimer_.Start(2.5f, false);
	alphaTimer_.Start(1.0f, false);
}

ThornParticle::~ThornParticle() {
	Cleanup();
}

void ThornParticle::Cleanup() {
	// 全パーティクルを非アクティブにする
	for (int i = 0; i < 10; i++) {
		pIsAlive_[i] = false;
		if (pModel_[i]) {
			pModel_[i].reset();
		}
	}

	// タイマーをリセット
	lifeTimer_.Reset();
	alphaTimer_.Reset();
}