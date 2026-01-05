#pragma once
#include "BaseChest.h"

class PaidChest : public BaseChest {
public:
	void Initialize(Vector3 pos) override;
	void Update() override;
	void Draw(Camera camera) override;
	void Open() override;
};