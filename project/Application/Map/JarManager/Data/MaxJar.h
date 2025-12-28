#pragma once
#include "BaseJar.h"

class MaxJar : public BaseJar {
	void Initialize(Vector3 pos) override;
	void Draw(Camera camera) override;
	int Break() override;
};