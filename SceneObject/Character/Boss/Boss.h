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

class Boss {
public:
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();
private:
	DirectXCommon* dxCommon_ = nullptr;

	unique_ptr<Object3d> body_ = make_unique<Object3d>();
	unique_ptr<Object3d> left_ = make_unique<Object3d>();
	unique_ptr<Object3d> right_ = make_unique<Object3d>();
	unique_ptr<Object3d> halo_ = make_unique<Object3d>();
	unique_ptr<Object3d> ringL_ = make_unique<Object3d>();
	unique_ptr<Object3d> ringR_ = make_unique<Object3d>();

	unique_ptr<Particles> leftFire_ = make_unique<Particles>();
	unique_ptr<Particles> rightFire_ = make_unique<Particles>();
	EmitterSphere fireEmitter_ = {};
	EmitterRange fireRange_ = {};

	const float bodyRadius_ = 3.0f;
	const float armRadius_ = 2.0f;

	void InitParticle();
};