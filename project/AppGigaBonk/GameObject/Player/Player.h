#pragma once
#include "AppGigaBonk/EngineSystem.h"
#include "AppGigaBonk/GameObject/GameObject.h"

class Player : GameObject
{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Player() {}

	void SetInputSystem(Input* inInput) { input_ = inInput; }
private:
	Input* input_ = nullptr;
};