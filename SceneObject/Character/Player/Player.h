#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Sprite.h"
#include"Camera.h"
#include"Gamepad.h"
#include"playerBullet.h"
#include"MatrixFunction.h"

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
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	GamePad gamePad_;

	float deltaTime = 1.0f / 60.0f;

	float speed_ = 15.0f;

	// bullet
	float bulletSpawnTimer_ = 0.0f;
	float bulletSpawnTime_ = 0.25f;
	Vector3 bulletVelocity_ = {};
	float bulletSpeed_ = 50.0f;

	// reticle
	float reticleSpeed_ = 640.0f;
	float kDistanceToReticle = 100.0f;
	
	// dash
	Vector3 rotate_ = {};
	float rotateSpeed_ = 0.0f;
	float rotateTimer_ = 0.0f;
	float rotateTime_ = 0.49f;
	float dashDirection_ = 0.0f;
	float dashCoolTime = 3.0f;
	float dashCoolTimer_ = 0.0f;
	bool isCanDash = true;

	// function
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