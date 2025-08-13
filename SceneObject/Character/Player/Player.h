#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Sprite.h"
#include"Camera.h"
#include"Gamepad.h"
#include"playerBullet.h"
#include"MatrixFunction.h"
#include"Particles.h"

#include <list>

using std::unique_ptr;
using std::make_unique;

class Player
{
public:
	
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();

	Vector3 GetWorldPosition();
private:
	DirectXCommon* dxCommon_ = nullptr;
	unique_ptr<Object3d> model_ = make_unique<Object3d>();
	unique_ptr<Object3d> reticle3D_ = make_unique<Object3d>();
	unique_ptr<Sprite> reticle2D_ = make_unique<Sprite>();
	unique_ptr<Particles> engineFire_ = make_unique<Particles>();
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	GamePad gamePad_;

	float deltaTime_ = 1.0f / 60.0f;

	float moveSpeed_ = 15.0f;
	Vector2 moveRotateSpeed_ = { 0.7f,0.5f };
	Vector3 moveRotate_ = {};

	//--- bullet ---//
	float bulletSpawnTimer_ = 0.0f;
	float bulletSpawnTime_ = 0.25f;
	Vector3 bulletVelocity_ = {};
	float bulletSpeed_ = 100.0f;

	//--- reticle ---//
	float reticleSpeed_ = 640.0f;
	float kDistanceToReticle = 100.0f;
	
	//--- dash ---//
	Vector3 dashRotate_ = {};
	float dashRotateSpeed_ = 0.0f;
	float dashRotateTimer_ = 0.0f;
	float dashRotateTime_ = 0.49f;
	float dashDirection_ = 0.0f;
	float dashCoolTime_ = 3.0f;
	float dashCoolTimer_ = 0.0f;
	bool isCanDash = true;

	//--- engineFire ---//
	EmitterSphere engineFireEmitter_ = {};
	EmitterRange engineFireEmitterRange_ = {};
	Vector3 engineFireOffset_ = { 0.0f,0.0f,-1.5f };

	//--- function ---//
	void Move();
	void Attack();
	void Action();
	void UpdateReticle(Camera* camera);

	enum State {
		NORMAL,
		DASH,
	};
	State state_ = NORMAL;
};