#include "stunP.h"

void StunP::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	for (size_t i = 0; i < stunParticle_.size(); i++) {
		stunParticle_[i] = std::make_unique<Particles>();
		stunParticle_[i]->Initialize(dxCommon_);
		stunParticle_[i]->LoadJson("stun");
		stunParticle_[i]->SetBlendMode(kBlendModeNormal);
		stunParticle_[i]->Stop();
	}

	stunParticle_[0]->SetEmitVelocity({ 5.0f,5.0f,0.0f });
	stunParticle_[1]->SetEmitVelocity({ -5.0f,5.0f,0.0f });
	stunParticle_[2]->SetEmitVelocity({ 5.0f,-5.0f,0.0f });
	stunParticle_[3]->SetEmitVelocity({ -5.0f,-5.0f,0.0f });
}

void StunP::Update() {
	for (size_t i = 0; i < stunParticle_.size(); i++) {
		stunParticle_[i]->Update();
	}
}

void StunP::Draw(Camera useCamera) {
	for (size_t i = 0; i < stunParticle_.size(); i++) {
		stunParticle_[i]->Draw(useCamera);
	}
}