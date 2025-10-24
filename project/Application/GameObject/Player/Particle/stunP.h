#pragma once
#include "Application/EngineSystem.h"
#include <array>

class StunP
{
public:
	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw(Camera useCamera);
	void Play(Vector3 pos) {
		for (size_t i = 0; i < stunParticle_.size(); i++) {
			stunParticle_[i]->SetEmitterPosition(pos);
			stunParticle_[i]->SetOffSet({ 0.0f,3.0f,0.0f });
			stunParticle_[i]->Play(false);
		}
	}
private:

	DirectXCommon* dxCommon_ = nullptr;
	Camera camera_;
	std::array<unique_ptr<Particles>,4> stunParticle_;
};