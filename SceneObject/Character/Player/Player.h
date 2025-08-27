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

#include"Audio.h"
#include"Input.h"

#include <list>

using std::unique_ptr;
using std::make_unique;

class Player
{
public:
	
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void DieUpdate(Camera* camera);

	void Draw();

	void DrawImGui();

	Vector3 GetWorldPosition();

	void Heal();

	void Damage();

	std::vector<PlayerBullet*> GetAllBullets();

	void BeamHit() { beam_->Hit(); }

	Vector3 BeamWorldPosition() { return beam_->GetWorldPosition(); }

	bool BeamIsAlive() { return beam_->GetIsAlive(); }

	void UseGamePad(bool useGamePad) { useGamePad_ = useGamePad; }

	bool IsDie() { if (life_ == 0) { return true; } else { return false; } }

	void CloseSound();

	void SetInput(Input* input) { input_ = input; }

	bool IsScreenOut() { return isScreenOut_; }
	bool IsScreenIn() { return isScreenIn_; }
private:
	DirectXCommon* dxCommon_ = nullptr;
	unique_ptr<Object3d> model_ = make_unique<Object3d>();
	unique_ptr<Object3d> reticle3D_ = make_unique<Object3d>();
	unique_ptr<Sprite> reticle2D_ = make_unique<Sprite>();
	unique_ptr<Sprite> lifeUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> gaugeUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> gauge_ = make_unique<Sprite>();
	unique_ptr<Sprite> menu_ = make_unique<Sprite>();
	unique_ptr<PlayerBeam> beam_ = make_unique<PlayerBeam>();
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	unique_ptr<AudioX> healSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> damageSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> dashSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> bShotSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> beamChargeSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> beamShotSE_ = make_unique<AudioX>();

	GamePad gamePad_;

	Input* input_ = nullptr;
	bool isScreenOut_ = false;
	bool isScreenIn_ = false;

	unique_ptr<Particles> dieFire_ = make_unique<Particles>();
	EmitterSphere dieFireEmitter_ = {};
	EmitterRange dieFireRange_ = {};

	unique_ptr<Particles> dieSmork_ = make_unique<Particles>();
	EmitterSphere dieSmorkEmitter_ = {};
	EmitterRange dieSmorkRange_ = {};

	float dieTySpeed_ = 2.5f;
	float dieTzSpeed_ = 7.5f;
	float dieRySpeed_ = 1.5f / 4.0f;

	float gaugePosX_ = 37.0f;
	float gaugeScaleX_ = 1.0f;

	bool isInvincible_ = false;
	float invincibleTimer_ = 0.0f;
	const float invincibleTime_ = 1.0f;

	bool useGamePad_ = false;
	int life_ = 3;

	float deltaTime_ = 1.0f / 60.0f;

	float moveSpeed_ = 15.0f;
	float minSpeed_ = 7.5f;
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
	float beamChargeTime_ = 1.0f;
	float beamChargeRadius_ = 0.0f;
	bool isBeamShot_ = false;
	Vector3 beamVelocity_ = {};

	//--- reticle ---//
	float reticleSpeed_ = 640.0f;
	float kDistanceToReticle = 80.0f;
	
	//--- dash ---//
	Vector3 dashRotate_ = {};
	float dashRotateSpeed_ = 0.0f;
	float dashRotateTimer_ = 0.0f;
	float dashRotateTime_ = 0.49f;
	float dashDirection_ = 0.0f;
	float dashCoolTime_ = 1.0f;
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
	void UpdateLife();

	enum State {
		NORMAL = 0,
		DASH = 1,
	};
	State state_ = NORMAL;

	public:
		State GetState() { return state_; }
};