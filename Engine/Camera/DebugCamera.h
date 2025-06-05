#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include "Camera.h"


class DebugCamera : public Camera {
public:
	DebugCamera();

	void Update() override;
private:
	
};