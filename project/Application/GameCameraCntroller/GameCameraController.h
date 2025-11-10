#pragma once
#include "Camera.h"

class GameCameraController
{
public:
	void Initialize();
	void Update();
	void SetTarget(Vector3 targetPosition) { camera_.SetPosition(targetPosition); }
	Camera& GetCamera() { return camera_; }
private:
	Camera camera_;
};