#include "GameCameraController.h"

void GameCameraController::Initialize(AppContext* ctx) {
	// AppContextを保存
	ctx_ = ctx;
	
	// カメラの初期設定
	camera_.SetTarget({0.0f, 0.0f, 0.0f});   // デフォルトターゲット
	camera_.SetDistance(30.0f);              // デフォルト距離
	camera_.SetTPSAngles(0.0f, 0.3f);        // デフォルト角度（少し上から見下ろす）
}

void GameCameraController::Update() {
	if (!ctx_) return;

	// ゲームパッドが接続されている場合のみ処理
	if (ctx_->gamePad.IsConnected()) {
		// 右スティックの入力を取得
		float rightStickX = ctx_->gamePad.RightStickX();
		float rightStickY = ctx_->gamePad.RightStickY();

		// 感度設定
		const float horizontalSensitivity = 0.05f;  // 水平方向の感度
		const float verticalSensitivity = 0.03f;    // 垂直方向の感度

		// スティック入力からカメラの角度を更新
		if (std::abs(rightStickX) > 0.1f || std::abs(rightStickY) > 0.1f) {
			// 水平角度：右スティックのX軸
			float deltaHorizontal = rightStickX * horizontalSensitivity;
			
			// 垂直角度：右スティックのY軸（反転）
			float deltaVertical = -rightStickY * verticalSensitivity;

			// カメラの角度を更新
			camera_.AddTPSAngles(deltaHorizontal, deltaVertical);
		}
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