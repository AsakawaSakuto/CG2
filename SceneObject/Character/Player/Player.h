#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Camera.h"

using std::unique_ptr;
using std::make_unique;

class Player
{
public:
	
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	unique_ptr<Object3d> model_ = make_unique<Object3d>();
};