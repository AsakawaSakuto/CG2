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
	
	Vector3 GetColor(int index) const {
		assert(index >= 0 && index < 5); // 範囲外チェック（任意）
		return { pModel_[index]->GetColor().x ,pModel_[index]->GetColor().y,pModel_[index]->GetColor().z };
	}

	Vector3 GetTranslate(int index) const {
		assert(index >= 0 && index < 5); // 範囲外チェック（任意）
		return pTransform_[index].translate;
	}

	bool GetLifeTimerFinish(int index) const {
		assert(index >= 0 && index < 5); // 範囲外チェック（任意）
		return lifeTimer_[index].IsFinished();
	}

	bool GetLifeTimerActive(int index) const {
		assert(index >= 0 && index < 5); // 範囲外チェック（任意）
		return lifeTimer_[index].IsActive(); // IsFinished()からIsActive()に変更
	}

	void ParticleReset(int index) {
		pIsAlive_[index] = false;
		pTransform_[index].scale = { 0.0f ,0.0f ,0.0f };
		lifeTimer_[index].Reset();
	}
private:
	
	std::array<std::unique_ptr<Model>, 5> pModel_;
	std::array<bool, 5> pIsAlive_ = { false };
	std::array<Transform, 5> pTransform_ = {};
	std::array<Vector3, 5> pVelocity_ = {};
	std::array<Vector3, 5> pRotateVelocity_ = {};
	GameTimer lifeTimer_[5] = {};
	GameTimer alphaTimer_ = {};

	float baseScale_ = 0.3f;
	float gravity_ = -0.125f;
	float acceleration_ = 0.0f;

	Vector3 colorRGB_;
	Random rand_;
};