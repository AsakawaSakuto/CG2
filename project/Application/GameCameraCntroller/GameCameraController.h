#pragma once
#include "Camera.h"
#include "Application/AppContext.h"
#include <cmath>

class GameCameraController
{
public:
	void Initialize(AppContext* ctx);
	void Update();
	
	// TPS機能
	void SetTarget(Vector3 targetPosition);
	void SetDistance(float distance);
	
	Camera& GetCamera() { return camera_; }

private:
	Camera camera_;
	AppContext* ctx_ = nullptr;
};