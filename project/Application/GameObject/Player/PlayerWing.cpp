#include "PlayerWing.h"

void PlayerWing::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "cube.obj");

	transform_.scale = {2.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};

	// 当たり判定更新(AABB)
	CollisionUpdate();

	isAlive_ = true;
}

void PlayerWing::Update() {
	if (!isAlive_)
		return;

	// 当たり判定更新(AABB)
	CollisionUpdate();

	model_->SetTransform(transform_);
	model_->Update();
}

void PlayerWing::Draw(Camera useCamera) { 
	if (!isAlive_)
		return;

	model_->Draw(useCamera); 
}

void PlayerWing::CollisionUpdate() {
	Vector3 t = transform_.translate;
	collisionAABB_.max = {t.x + 1.0f, t.y + 0.5f, t.z + 0.5f};
	collisionAABB_.min = {t.x - 1.0f, t.y - 0.5f, t.z - 0.5f};
}
