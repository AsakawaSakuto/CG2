#pragma once
#include "Application/GameObject/GameObject.h"
#include <memory>

class Bullet : public GameObject {
public:

	void Initialize(DirectXCommon* dxCommon) override;
	void Initialize(DirectXCommon* dxCommon, Vector3 translate);
	void Update() override;
	void Draw(Camera useCamera) override;
	void Draw(Camera useCamera, Model* model);

	// Getter
	float GetSpeed() const { return speed_; }
	Transform& GetTransform() { return transform_; }

	// Setter
	void SetVelocity(float velocity);

private:
	float speed_ = 10.0f;
	float velocity_ = 0.0f;
};
