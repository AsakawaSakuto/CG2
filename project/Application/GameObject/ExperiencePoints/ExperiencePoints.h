#pragma once
#include "Application/GameObject/GameObject.h"

class ExperiencePoints : public GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~ExperiencePoints() {}

private:

};
