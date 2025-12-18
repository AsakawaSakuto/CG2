#include "GameCameraController.h"
#include "Input/KeyConfig.h"

void GameCameraController::Initialize() {
	// カメラの初期設定
	camera_.SetTarget({0.0f, 0.0f, 0.0f});   // デフォルトターゲット
	camera_.SetDistance(30.0f);              // デフォルト距離
	camera_.SetTPSAngles(0.0f, 0.3f);        // デフォルト角度（少し上から見下ろす）
}

void GameCameraController::Update() {

	// KeyConfigを使ってカメラ操作の入力を取得
	auto cameraInput = MyInput::GetVector2D(Action::CAMERA_LOOK);

	// 感度設定
	const float horizontalSensitivity = 0.05f;  // 水平方向の感度
	const float verticalSensitivity = 0.03f;    // 垂直方向の感度

	// スティック入力からカメラの角度を更新
	if (std::abs(cameraInput.x) > 0.1f || std::abs(cameraInput.y) > 0.1f) {
		// 水平角度：スティックのX軸
		float deltaHorizontal = cameraInput.x * horizontalSensitivity;
		
		// 垂直角度：スティックのY軸（反転）
		float deltaVertical = -cameraInput.y * verticalSensitivity;

		// カメラの角度を更新
		camera_.AddTPSAngles(deltaHorizontal, deltaVertical);
	}

	// TPSカメラの更新（地面貫通の制限はCalculateTPSPosition内で処理される）
	camera_.UpdateTPS();
}

void GameCameraController::SetTarget(Vector3 targetPosition) {
	targetPosition_ = targetPosition;
	camera_.SetTarget(targetPosition_);
}

void GameCameraController::SetDistance(float distance) {
	camera_.SetDistance(distance);
}