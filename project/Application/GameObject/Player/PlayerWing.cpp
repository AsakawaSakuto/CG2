#include "PlayerWing.h"

void PlayerWing::Initialize(DirectXCommon* dxCommon) {

}

void PlayerWing::Initialize(DirectXCommon* dxCommon, Vector3 translate)
{
	dxCommon_ = dxCommon;

	transform_.scale = {4.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = translate;

	// 当たり判定(AABB)
	Vector3 t = transform_.translate;
	CollisionAABB_.max = {t.x + 2.0f, t.y + 1.0f, t.z + 1.0f};
	CollisionAABB_.min = {t.x - 2.0f, t.y - 1.0f, t.z - 1.0f};
}

void PlayerWing::Update() {

}

void PlayerWing::Draw(Camera useCamera) {

}
