#include "Application/GameObject/Thorn/Particle/ThornParticle.h"

void ThornParticle::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	for (int i = 0; i < 5; i++) {
		pModel_[i] = std::make_unique<Model>();
		pModel_[i]->Initialize(dxCommon_, "Candy/Candy.obj");
		pModel_[i]->SetUpdateFrustumCulling(false);

		pTransform_[i].scale = { baseScale_,baseScale_,baseScale_ };
		pTransform_[i].rotate = { 0.0f,0.0f,0.0f };
		pTransform_[i].translate = { 0.0f,0.0f,0.0f };
	}
}

void ThornParticle::Update() {

	for (int i = 0; i < 5; i++) {
		if (!pIsAlive_[i]) {
			return;
		}

		pTransform_[i].translate += pVelocity_[i];

		pTransform_[i].rotate.x += pRotateVelocity_[i].x;
		pTransform_[i].rotate.y += pRotateVelocity_[i].y;
		pTransform_[i].rotate.z += pRotateVelocity_[i].z;

		acceleration_ += gravity_ * deltaTime_;
		pVelocity_[i].y += acceleration_ * deltaTime_;

		pModel_[i]->SetTransform(pTransform_[i]);

		pModel_[i]->SetColor({ colorRGB_.x, colorRGB_.y, colorRGB_.z, alphaTimer_.GetReverseProgress() });

		pModel_[i]->Update();

		if (lifeTimer_[i].IsFinished()) {
			pIsAlive_[i] = false;
			pTransform_[i].scale = { 0.0f ,0.0f ,0.0f };
			lifeTimer_[i].Reset();
		}

		lifeTimer_[i].Update();
	}

	alphaTimer_.Update();
}

void ThornParticle::Draw(Camera useCamera) {
	for (int i = 0; i < 5; i++) {
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

		colorRGB_.x = rand_.Float(0.3f, 0.8f);
		colorRGB_.y = rand_.Float(0.3f, 0.8f);
		colorRGB_.z = rand_.Float(0.3f, 0.8f);

		pTransform_[i].translate = pos;
		pTransform_[i].scale = { baseScale_,baseScale_,baseScale_ };

		pIsAlive_[i] = true;

		acceleration_ = 0.0f;

		lifeTimer_[i].Start(2.0f, false);
	}
	alphaTimer_.Start(1.0f, false);
}

ThornParticle::~ThornParticle() {
	Cleanup();
}

void ThornParticle::Cleanup() {
	// 全パーティクルを非アクティブにする
	for (int i = 0; i < 5; i++) {
		pIsAlive_[i] = false;
		if (pModel_[i]) {
			pModel_[i].reset();
		}
		lifeTimer_[i].Reset();
	}

	// タイマーをリセット
	alphaTimer_.Reset();
}