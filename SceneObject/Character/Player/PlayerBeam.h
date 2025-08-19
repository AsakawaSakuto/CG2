#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Particles.h"
#include"Camera.h"

using std::unique_ptr;
using std::make_unique;

class PlayerBeam
{
public:
	
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();

	void Spawn(Vector3 translate, Vector3 velocity);
	
	void Hit();

	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }

	bool GetIsAlive() { return isAlive_; }

	Vector3 GetWorldPosition() { return model_->GetWorldPosition(); }

	void SetEmitterRange(EmitterRange range) { beamRange_ = range; }
private:
	DirectXCommon* dxCommon_ = nullptr;

	unique_ptr<Object3d> model_ = make_unique<Object3d>();

	unique_ptr<Particles> pBeam_ = make_unique<Particles>();
	EmitterSphere beamEmitter_ = {};
	EmitterRange beamRange_ = {};

	float deltaTime_ = 1.0f / 60.0f;
	bool isAlive_ = false;

	float time_ = 1.0f;
	float timer_ = 0.0f;

	float deleteTime_ = 0.15f;
	float deleteTimer_ = 0.0f;
	bool isDelete_ = false;

	float speed_ = 300.0f;
	Vector3 velocity_;
};
