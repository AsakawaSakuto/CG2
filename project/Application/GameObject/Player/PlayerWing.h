#pragma once
#include "Application/GameObject/GameObject.h"

class PlayerWing : public GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~PlayerWing() {}

	// Setter
	void SetPosition(Vector3 position) { transform_.translate = position; }
	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }

private:
	// 当たり判定更新(AABB)
	void CollisionUpdate();
};
