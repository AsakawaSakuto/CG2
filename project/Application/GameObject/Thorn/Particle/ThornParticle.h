#pragma once
#include "Application/GameObject/GameObject.h"
#include <array>

class ThornParticle : GameObject
{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	void Play(Vector3 pos, uint32_t playNum);
	~ThornParticle();
	
	// リソースクリーンアップ
	void Cleanup();
	
private:
	
	std::array<std::unique_ptr<Model>, 5> pModel_;
	std::array<bool, 5> pIsAlive_ = { false };
	std::array<Transform, 5> pTransform_ = {};
	std::array<Vector3, 5> pVelocity_ = {};
	std::array<Vector3, 5> pRotateVelocity_ = {};
	GameTimer lifeTimer_[5] = {};
	GameTimer alphaTimer_ = {};

	float baseScale_ = 0.2f;
	float gravity_ = -0.125f;
	float acceleration_ = 0.0f;

	Vector3 colorRGB_;
	Random rand_;
};