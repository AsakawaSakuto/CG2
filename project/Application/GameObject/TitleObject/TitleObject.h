#pragma once
#include "Application/GameObject/GameObject.h"
#include <array>

class TitleObject {
public:
	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw(Camera& useCamera);

	void DrawImGui();

	void PlayerStart() { playerInTimer_.Start(1.0f); }
	void PlayerEnd() { playerOutTimer_.Start(1.0f); ramuneParticle_->SetSpawnCount(30); }
	~TitleObject() {}
private:
	void InitTransform();

	DirectXCommon* dxCommon_ = nullptr;

	GameTimer playerInTimer_;
	GameTimer playerOutTimer_;

	std::array<std::unique_ptr<Model>, 11> model_;
	std::array<Transform, 11> transform_;

	std::unique_ptr<Particles> ramuneParticle_ = make_unique<Particles>();

	// ふわふわアニメーション用
	float floatTime_ = 0.0f;
	float floatAmplitude_ = -0.3f; // 上下の振幅
	float floatSpeed_ = 2.0f;      // 浮遊速度
};