#pragma once
#include "BaseChest.h"

class FreeChest : public BaseChest {
public:
	void Initialize(Vector3 pos) override;
	void Draw(Camera camera) override;
	void Open() override;
};