#include "Thorn.h"

void Thorn::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "sphere.obj");

	transform_.scale = {0.5f, 0.5f, 0.5f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};

	// 当たり判定(AABB)
	Vector3 t = transform_.translate;
	collisionAABB_.max = {t.x + 1.0f, t.y + 1.0f, t.z + 1.0f};
	collisionAABB_.min = {t.x - 1.0f, t.y - 1.0f, t.z - 1.0f};
	collisionSphere_.radius = 0.25f;
}

void Thorn::Update() {
	if (!isAlive_)
		return;

	// トゲのサイズを更新
	UpgradeThorn();

	model_->SetTransform(transform_);
	model_->Update();
}

void Thorn::Draw(Camera useCamera) { model_->Draw(useCamera); }

void Thorn::Spawn(Vector3 position) {
	if (isAlive_)
		return;
	else {
		isAlive_ = true;
		transform_.translate = position;
		collisionSphere_.center = transform_.translate;
	}
}

void Thorn::UpgradeThorn() { 
	switch (type_)
	{ 
	case ThornType::MIN:
		transform_.scale = {0.5f, 0.5f, 0.5f};
		collisionSphere_.radius = 0.25f;
		break;
	case ThornType::MIDDLE:
		transform_.scale = {0.7f, 0.7f, 0.7f};
		collisionSphere_.radius = 0.35f;
		break;
	case ThornType::MAX:
		transform_.scale = {1.0f, 1.0f, 1.0f};
		collisionSphere_.radius = 0.5f;
		break;
	}
}
