#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Sprite.h"
#include"Camera.h"
#include"Gamepad.h"
#include"playerBullet.h"
#include"playerBeam.h"
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

	void Heal();

	void Damage();
private:
	DirectXCommon* dxCommon_ = nullptr;
	unique_ptr<Object3d> model_ = make_unique<Object3d>();
	unique_ptr<Object3d> reticle3D_ = make_unique<Object3d>();
	unique_ptr<Sprite> reticle2D_ = make_unique<Sprite>();
	unique_ptr<PlayerBeam> beam_ = make_unique<PlayerBeam>();
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	GamePad gamePad_;

	int life_ = 3;

	float deltaTime_ = 1.0f / 60.0f;

	float moveSpeed_ = 15.0f;
	float minSpeed_ = 10.0f;
	float maxSpeed_ = 15.0f;

	Vector2 moveRotateSpeed_ = { 0.7f,0.7f };
	Vector3 moveRotate_ = {};

	//--- bullet ---//
	float bulletSpawnTimer_ = 0.0f;
	float bulletSpawnTime_ = 0.25f;
	Vector3 bulletVelocity_ = {};
	float bulletSpeed_ = 100.0f;

	//--- baem ---//
	float beamChargeTimer_ = 0.0f;
	float beamChargeTime_ = 1.5f;
	float beamChargeRadius_ = 0.0f;
	bool isBeamShot_ = false;
	Vector3 beamVelocity_ = {};

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

	//--- particle ---//
	unique_ptr<Particles> engineFire_ = make_unique<Particles>();
	EmitterSphere engineFireEmitter_ = {};
	EmitterRange engineFireRange_ = {};
	Vector3 engineFireOffset_ = { 0.0f,0.0f,-1.5f };

	unique_ptr<Particles> heal_ = make_unique<Particles>();
	EmitterSphere healEmitter_ = {};
	EmitterRange healRange_ = {};
	float healTime_ = 0.75f;
	float healTimer_ = 0.0f;
	bool isHeal_ = false;

	unique_ptr<Particles> damage_ = make_unique<Particles>();
	EmitterSphere damageEmitter_ = {};
	EmitterRange damageRange_ = {};
	float damageTime_ = 0.1f;
	float damageTimer_ = 0.0f;
	bool isDamage_ = false;

	unique_ptr<Particles> beamCharge_ = make_unique<Particles>();
	EmitterSphere beamChargeEmitter_ = {};
	EmitterRange beamChargeRange_ = {};
	Vector3 beamChargeOffset_ = { 0.0f,0.0f,2.0f };

	//--- function ---//
	void Move();
	void Attack();
	void Action();
	void UpdateReticle(Camera* camera);
	void UpdateParticle();

	enum State {
		NORMAL,
		DASH,
	};
	State state_ = NORMAL;
};