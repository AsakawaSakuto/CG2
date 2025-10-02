#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Sprite.h"
#include"Camera.h"
#include"Gamepad.h"
#include"MatrixFunction.h"
#include"Particles.h"

#include"BulletState.h"
#include"Audio.h"
#include"Engine/System/Utility/Random/Random.h"

#include <list>

using std::unique_ptr;
using std::make_unique;

class BossBullet {
public:
	
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void Spawn(Vector3 translate, Vector3 velocity);

	void Hit();

	bool GetIsAlive() { return isAlive_; }

	void SetVelocity(Vector3 velo) { velocity_ = velo; }

	void SetTranslate(Vector3 t) { model_->SetTranslate(t);heal_->SetTranslate(t);}

	bool GetIsMove() { return isMove_; }

	Vector3 GetPos();

	BulletState GetBulletState() { return bState_; }

	void CloseSound() { bShotSE_->Reset(); }
private:
	DirectXCommon* dxCommon_ = nullptr;

	unique_ptr<AudioX> bShotSE_ = make_unique<AudioX>();

	unique_ptr<Object3d> model_ = make_unique<Object3d>();
	unique_ptr<Object3d> heal_ = make_unique<Object3d>();

	const float deltaTime_ = 1.0f / 60.0f;

	bool isAlive_ = false;
	bool isMove_ = false;

	Vector3 velocity_ = {};
	Vector3 scale_ = { 0.0f,0.0f,0.0f };

	const float speed_ = 150.0f;

	const float lifeTime_ = 3.0f;
	float lifeTimer_ = 0.0f;

	BulletState bState_ = DAMAGE;
	int randNum_ = 0;
	Random rand_;
};