#pragma once
#include "BaseJar.h"

class MinJar : public BaseJar {
	void Initialize(Vector3 pos) override;
	void Draw(Camera camera) override;
	int Break() override;
};