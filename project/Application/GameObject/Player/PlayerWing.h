#pragma once
#include "Application/GameObject/GameObject.h"

class PlayerWing : GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Initialize(DirectXCommon* dxCommon, Vector3 translate);
	void Update() override;
	void Draw(Camera useCamera) override;
	~PlayerWing() {}

private:

};
