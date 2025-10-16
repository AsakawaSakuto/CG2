#pragma once
#include "Application/GameObject/GameObject.h"
#include <array>

class ThornParticle : GameObject
{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~ThornParticle() {}
private:
	std::array<std::unique_ptr<Model>, 10> pModels_;
	std::array<bool, 10> pIsAlive_ = { false };
	std::array<Transform, 10> pTransform_ = {};
};