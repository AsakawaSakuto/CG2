#include "GameCameraController.h"
#include "Input/MyInput.h"
#include <imgui.h>

void GameCameraController::Initialize() {
	// カメラの初期設定
	camera_.SetTarget({0.0f, 0.0f, 0.0f});   // デフォルトターゲット
	camera_.SetDistance(27.5f);              // デフォルト距離
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

	// オフセットを適用したターゲット位置を設定
	camera_.SetTarget(targetPosition_ + offset_);

	// TPSカメラの更新（地面貫通の制限はCalculateTPSPosition内で処理される）
	camera_.UpdateTPS();
}

void GameCameraController::SetTarget(Vector3 targetPosition) {
	targetPosition_ = targetPosition;
	camera_.SetTarget(targetPosition_ + offset_);
}

void GameCameraController::SetDistance(float distance) {
	camera_.SetDistance(distance);
}

void GameCameraController::DrawImgui() {
#ifdef  USE_IMGUI

	if (ImGui::TreeNode("GameCameraController")) {
		// ターゲット位置の調整
		ImGui::DragFloat3("Target Position", &targetPosition_.x, 0.1f);
		
		// オフセットの調整
		if (ImGui::DragFloat3("Offset", &offset_.x, 0.1f)) {
			camera_.SetTarget(targetPosition_ + offset_);
		}
		
		// カメラの距離調整
		float distance = camera_.GetDistance();
		if (ImGui::DragFloat("Distance", &distance, 0.1f, 1.0f, 100.0f)) {
			camera_.SetDistance(distance);
		}
		
		// カメラの角度調整
		float horizontalAngle = camera_.GetHorizontalAngle();
		float verticalAngle = camera_.GetVerticalAngle();
		
		if (ImGui::DragFloat("Horizontal Angle", &horizontalAngle, 0.01f)) {
			camera_.SetTPSAngles(horizontalAngle, verticalAngle);
		}
		
		if (ImGui::DragFloat("Vertical Angle", &verticalAngle, 0.01f, -1.5f, 1.5f)) {
			camera_.SetTPSAngles(horizontalAngle, verticalAngle);
		}
		
		// カメラ内部のImGuiも表示
		camera_.DrawImgui();
		
		ImGui::TreePop();
	}

#endif //  USE_IMGUI
}