#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Sprite.h"
#include"Camera.h"
#include"Gamepad.h"
#include"MatrixFunction.h"
#include"Particles.h"

#include"BossBullet.h"
#include"BossArm.h"

#include <list>

using std::unique_ptr;
using std::make_unique;

class Boss {
public:
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();

	Vector3 GetBodyWorldPos() { return body_->GetWorldPosition(); }

	void SetBodyTlansrate(Vector3 tlansrate) { body_->SetTranslate(tlansrate); }

	void UseFire(bool use) { leftFire_->UseEmitter(use); rightFire_->UseEmitter(use); }

	void SetPlayerPos(Vector3 pPos) { playerPos_ = pPos; }

	void SetIsStart(bool isStart) { isStart_ = isStart; }

	Vector3 GetBossBulletPos() { return bullet_->GetPos(); }

	Vector3 GetArmPosL() { return arm_->GetArmPosL(); }
	Vector3 GetArmPosR() { return arm_->GetArmPosR(); }

	void SetBodyColor(Vector4 color) { body_->SetColor(color); }

	void BulletHit() { bullet_->Hit(); }

	void Damage() { life_--; };

	bool IsDie() { if (life_ <= 0) { return true; } else { return false; } }

	BulletState GetBulletState() { return bullet_->GetBulletState(); }
private:
	int life_ = 10;

	DirectXCommon* dxCommon_ = nullptr;

	unique_ptr<Object3d> body_ = make_unique<Object3d>();
	unique_ptr<Object3d> halo_ = make_unique<Object3d>();
	unique_ptr<Object3d> ringL_ = make_unique<Object3d>();
	unique_ptr<Object3d> ringR_ = make_unique<Object3d>();

	unique_ptr<Particles> leftFire_ = make_unique<Particles>();
	unique_ptr<Particles> rightFire_ = make_unique<Particles>();

	unique_ptr<BossBullet> bullet_ = make_unique<BossBullet>();
	unique_ptr<BossArm> arm_ = make_unique<BossArm>();

	unique_ptr<Sprite> hpUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> hpUI2_ = make_unique<Sprite>();

	EmitterSphere fireEmitter_ = {};
	EmitterRange fireRange_ = {};

	Vector3 playerPos_ = {};

	const float bodyRadius_ = 3.0f;
	const float armRadius_ = 2.0f;

	Vector2 speed_ = { 10.0f,5.0f };

	float time_ = 0.0f;
	Vector3 startPosition_ = {};

	bool isStart_ = false;

	const float deltaTime_ = 1.0f / 60.0f;

	float haloRy_ = 0.0f;
	float haloSpinSpeed_ = 3.0f;
	float bulletShotTimer_ = 0.0f;
	const float bulletShotTime_ = 5.0f;
	bool haloIsShot_ = false;

	void InitParticle();
	void UpdateHalo();
};