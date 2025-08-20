#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Sprite.h"
#include"Camera.h"
#include"Gamepad.h"
#include"playerBullet.h"
#include"playerBeam.h"
#include"MatrixFunction.h"
#include"Particles.h"

#include <list>

using std::unique_ptr;
using std::make_unique;

class Boss {
public:
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();
private:
	DirectXCommon* dxCommon_ = nullptr;
};