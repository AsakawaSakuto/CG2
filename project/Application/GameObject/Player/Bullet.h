#pragma once
#include "Application/GameObject/GameObject.h"
#include <memory>

class Bullet : public GameObject {
public:

	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;

	void Spawn(Vector3 position, float velocity);

	// Getter
	float GetSpeed() const { return speed_; }
	Transform& GetTransform() { return transform_; }

private:
	float speed_ = 15.0f;
	float velocity_ = 0.0f;
};
