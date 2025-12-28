#pragma once
#include "Camera/Camera.h"
#include <cmath>

class GameCameraController
{
public:
	void Initialize();
	void Update();
	void DrawImgui();
	
	// TPS機能
	void SetTarget(Vector3 targetPosition);
	void SetDistance(float distance);
	
	Camera& GetCamera() { return camera_; }

private:
	Camera camera_;
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 offset_ = { 0.0f, 4.0f, 0.0f };
	
	bool isMouseCameraActive_ = false; // マウスカメラ操作がアクティブかどうか
};