#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "Input.h"

class DebugCamera : public Camera {
public:
	DebugCamera();

	void Update() override;

	void SetInput(Input* inInput) { input_ = inInput; }

	void DrawImgui();
private:
	enum MoveDirection {
		MOVE_X,
		MODE_Y,
		MOVE_Z,
		COUNT,
	};
	MoveDirection moveDirection = MOVE_Z;
	Input* input_ = nullptr;
	float wheel;
	float scrollSpeed;
};