#pragma once
#include "Utility/Transform/Transform.h"
#include "Math/Type/Matrix4x4.h"
#include "Camera/Camera.h"
#include "KeyConfig/InputDevice/Input.h"
#include "Utility/FileFormat/Binary/BinaryManager.h"

class DebugCamera : public Camera {
public:
	enum class CameraMode {
		OrbitAroundOrigin,  // 原点注視モード
		FreeRotation        // 自由回転モード
	};

	DebugCamera();

	void Update() override;

	void SetInput(Input* inInput) { input_ = inInput; }

	void DrawImgui();
	
	// モード切り替え
	void SetCameraMode(CameraMode mode) { cameraMode_ = mode; }
	CameraMode GetCameraMode() const { return cameraMode_; }
	void ToggleCameraMode();

private:

	void Save();

	Input* input_ = nullptr;
	float scrollSpeed;
	float moveSpeedMultiplier;
	float rotateSpeedMultiplier;
	
	// カメラモード
	CameraMode cameraMode_;
	
	// Blender式カメラ用（原点注視モード）
	Vector3 targetPosition_;  // 注視点（原点）
	float distance_;          // 注視点からの距離
	float horizontalAngle_;   // 水平角度
	float verticalAngle_;     // 垂直角度
	
	// カメラ位置を計算
	void UpdateCameraPositionOrbit();    // 原点注視モード

	std::unique_ptr<BinaryManager> binaryManager_;
	const std::string binaryFileName_ = "debugCameraState";
};