#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Sprite.h"
#include"Camera.h"
#include"Gamepad.h"
#include"MatrixFunction.h"
#include"Particles.h"

#include <list>

using std::unique_ptr;
using std::make_unique;

class BossArm
{
public:

	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();

	void SetBossPos(Vector3 bPos) { bossPos_ = bPos; }

	void InitParticle();
private:
	DirectXCommon* dxCommon_ = nullptr;

	const float deltaTime_ = 1.0f / 60.0f;

	unique_ptr<Object3d> armL_ = make_unique<Object3d>();
	unique_ptr<Object3d> armR_ = make_unique<Object3d>();

	unique_ptr<Particles> leftFire_ = make_unique<Particles>();
	unique_ptr<Particles> rightFire_ = make_unique<Particles>();
	EmitterSphere fireEmitterL_ = {};
	EmitterRange fireRangeL_ = {};
	EmitterSphere fireEmitterR_ = {};
	EmitterRange fireRangeR_ = {};


	Vector3 bossPos_ = {};

	Vector3 armPosL_ = {};
	Vector3 armPosR_ = {};

	const float startSpeed_ = 10.0f;

	bool isAction = false;
	float actionTimer_ = 0.0f;
	const float actionTime_ = 15.0f;

	bool isChange = false;
	float changeTimer_ = 0.0f;
	const float changeTime_ = 5.0f;
};