#pragma once
#include "Application/GameObject/GameObject.h"

class Temp : GameObject
{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Temp() {}
private:
};