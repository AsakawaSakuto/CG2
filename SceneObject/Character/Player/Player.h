#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Camera.h"
#include"Gamepad.h"

using std::unique_ptr;
using std::make_unique;

class Player
{
public:
	
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();
private:
	DirectXCommon* dxCommon_ = nullptr;
	unique_ptr<Object3d> model_ = make_unique<Object3d>();
	GamePad gamePad_;

	float deltaTime = 1.0f / 60.0f;
	float speed_ = 10.0f;

	void Move();
};