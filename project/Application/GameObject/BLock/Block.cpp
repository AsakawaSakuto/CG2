#include "Block.h"
void Block::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "cube.obj");

	transform_.scale = {0.5f, 0.5f, 0.5f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};

	// 当たり判定(AABB)
	Vector3 t = transform_.translate;
	collisionAABB_.max = {t.x + 1.0f, t.y + 1.0f, t.z + 1.0f};
	collisionAABB_.min = {t.x - 1.0f, t.y - 1.0f, t.z - 1.0f};
}

void Block::Update() {
	if (!isAlive_)
		return;

	model_->SetTransform(transform_);
	model_->Update();
}

void Block::Draw(Camera useCamera) { model_->Draw(useCamera); }

void Block::Spawn(Vector3 position) {
	if (isAlive_)
		return;
	else {
		isAlive_ = true;
		transform_.translate = position;
		collisionSphere_.center = transform_.translate;
	}
}