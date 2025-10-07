#pragma once
#include "Application/GameObject/GameObject.h"

class Thorn: GameObject
{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Thorn() {}
};
