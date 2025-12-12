#pragma once
#include "Utility/Transform/Transform.h"
#include "Math/Type/Matrix4x4.h"
#include "Camera/Camera.h"
#include "Input/Input.h"

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
	float moveSpeedMultiplier;
	float rotateSpeedMultiplier;
};