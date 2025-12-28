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

	// マウスの移動量を取得
	auto mouseDelta = MyInput::GetInput()->GetMouseDelta();

	// 感度設定
	const float horizontalSensitivity = 0.05f;  // 水平方向の感度（スティック）
	const float verticalSensitivity = 0.03f;    // 垂直方向の感度（スティック）
	const float mouseHorizontalSensitivity = 0.003f;  // 水平方向の感度（マウス）
	const float mouseVerticalSensitivity = 0.003f;    // 垂直方向の感度（マウス）

	float deltaHorizontal = 0.0f;
	float deltaVertical = 0.0f;

	// スティック入力からカメラの角度を更新
	if (std::abs(cameraInput.x) > 0.1f || std::abs(cameraInput.y) > 0.1f) {
		// 水平角度：スティックのX軸
		deltaHorizontal += cameraInput.x * horizontalSensitivity;
		
		// 垂直角度：スティックのY軸（反転）
		deltaVertical += -cameraInput.y * verticalSensitivity;
	}

	// マウス右クリックでカメラ操作モードをトグル（ON/OFF切り替え）
	auto* input = MyInput::GetInput();
	if (input && input->GetWinApp()) {
		// 右クリックのトリガー（押した瞬間）でトグル
		if (input->TriggerMouseButtonR()) {
			isMouseCameraActive_ = !isMouseCameraActive_;
			
			if (isMouseCameraActive_) {
				// マウスカメラモードをON
				ShowCursor(FALSE);
			} else {
				// マウスカメラモードをOFF
				ShowCursor(TRUE);
			}
		}

		// マウスカメラモードがアクティブの場合
		if (isMouseCameraActive_) {
			// マウス入力からカメラの角度を更新
			if (std::abs(mouseDelta.x) > 0.01f || std::abs(mouseDelta.y) > 0.01f) {
				// 水平角度：マウスのX軸移動
				deltaHorizontal += mouseDelta.x * mouseHorizontalSensitivity;
				
				// 垂直角度：マウスのY軸移動
				deltaVertical += mouseDelta.y * mouseVerticalSensitivity;
			}

			// マウスを画面中央にリセット
			HWND hwnd = input->GetWinApp()->GetHWND();
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			int centerX = (clientRect.right - clientRect.left) / 2;
			int centerY = (clientRect.bottom - clientRect.top) / 2;
			
			POINT centerPoint = { centerX, centerY };
			ClientToScreen(hwnd, &centerPoint);
			SetCursorPos(centerPoint.x, centerPoint.y);
			
			// Inputクラスの内部マウス座標も更新
			input->SetMousePosition(centerX, centerY);
		}
	}

	// カメラの角度を更新
	if (deltaHorizontal != 0.0f || deltaVertical != 0.0f) {
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
		// マウスカメラモードの状態表示
		ImGui::Text("Mouse Camera Mode: %s", isMouseCameraActive_ ? "ON" : "OFF");
		ImGui::Text("(Right Click to Toggle)");
		ImGui::Separator();
		
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