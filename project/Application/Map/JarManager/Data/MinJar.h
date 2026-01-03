#pragma once
#include "BaseJar.h"

class MinJar : public BaseJar {
public:
	void Initialize(Vector3 pos) override;
	void Update() override;
	void Draw(Camera camera) override;
	int Break() override;
};