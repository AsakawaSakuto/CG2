#pragma once
#include "AppGigaBonk/EngineSystem.h"
#include "AppGigaBonk/GameObject/GameObject.h"

class Temp : GameObject
{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Temp() {}
private:
};