#include "Application/GameObject/Thorn/Particle/ThornParticle.h"

void ThornParticle::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	for (int i = 0; i < 10; i++) {
		pModel_[i] = std::make_unique<Model>();
		pModel_[i]->Initialize(dxCommon_, "Candy/Candy.obj");
	}
}

void ThornParticle::Update() {

	for (int i = 0; i < 10; i++) {
		pModel_[i]->Update();
	}
}

void ThornParticle::Draw(Camera useCamera) {
	for (int i = 0; i < 10; i++) {
		if (pIsAlive_[i]) {
			pModel_[i]->Draw(useCamera);
		}
	}
}

void ThornParticle::Play() {
	Random rand;
	for (int i = 0; i < 10; i++) {
		pVelocity_[i].x = rand.Float(-0.1f, 0.1f);
	}
}