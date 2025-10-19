#pragma once
#include "Application/GameObject/GameObject.h"

class Bear : public GameObject{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Bear() {}

private:
	std::unique_ptr<Model> modelBearHead_;
};
