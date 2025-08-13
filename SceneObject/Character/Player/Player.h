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

	GamePad gamePad_;

	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	float bulletSpawnTimer_ = 0.0f;
	float bulletSpawnTime_ = 0.25f;
	Vector3 bulletVelocity_ = {};
	float bulletSpeed_ = 50.0f;

	float deltaTime = 1.0f / 60.0f;
	float speed_ = 15.0f;

	float reticleSpeed_ = 640.0f;
	float kDistanceToReticle = 100.0f;

	void Move();
	void UpdateReticle(Camera* camera);
	void Attack();
};