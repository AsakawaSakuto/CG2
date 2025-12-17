#include "DebugCamera.h"
#include <cmath>

#ifdef USE_IMGUI
#include "imgui.h"           
#include "imgui_impl_dx12.h" 
#include "imgui_impl_win32.h"
#endif

DebugCamera::DebugCamera() {
	moveSpeedMultiplier = 1.0f;
	rotateSpeedMultiplier = 1.0f;
	scrollSpeed = 1.0f;

	cameraMode_ = CameraMode::OrbitAroundOrigin;

	targetPosition_ = { 0.0f, 0.0f, 0.0f };
	distance_ = 30.0f;

	horizontalAngle_ = 3.135f; // 画像の値に寄せる（ほぼπ）
	verticalAngle_ = 0.200f;   // 少し上から見下ろす

	UpdateCameraPositionOrbit();

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(
		fovY_,
		static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_),
		nearClip_, farClip_);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);

	binaryManager_ = std::make_unique<BinaryManager>();

	auto data = binaryManager_->Read(binaryFileName_);
	int index = 0;

	if (data.size() < index + 3) {
		return;
	}

	scrollSpeed = BinaryManager::Reverse<float>(data[index++]);
	moveSpeedMultiplier = BinaryManager::Reverse<float>(data[index++]);
	rotateSpeedMultiplier = BinaryManager::Reverse<float>(data[index++]);
}


void DebugCamera::Update() {
#ifdef USE_IMGUI
	// ImGuiがマウスを使用している場合は処理をスキップ
	if (ImGui::GetIO().WantCaptureMouse) {
		worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
		viewMatrix_ = InverseMatrix(worldMatrix_);
		viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
		return;
	}
#endif

	// Tキーでモード切り替え
	if (input_->TriggerKey(DIK_T)) {
		ToggleCameraMode();
	}

	// モードに応じた操作
	if (cameraMode_ == CameraMode::OrbitAroundOrigin) {
		// 原点注視モード
		
		// 回転: 中ボタンドラッグ
		if (input_->PushMouseButtonM() && !input_->PushKey(DIK_LSHIFT)) {
			Vector2 delta = input_->GetMouseDelta();
			float rotateSpeed = 0.005f;
			horizontalAngle_ -= delta.x * rotateSpeed * rotateSpeedMultiplier;
			verticalAngle_ -= delta.y * rotateSpeed * rotateSpeedMultiplier;
			
			// 垂直角度を制限（真上と真下を避ける）
			const float maxVertical = 1.5f;
			const float minVertical = -1.5f;
			if (verticalAngle_ > maxVertical) verticalAngle_ = maxVertical;
			if (verticalAngle_ < minVertical) verticalAngle_ = minVertical;
		}

		// 平行移動 (Pan): Shift + 中ボタンドラッグ
		if (input_->PushMouseButtonM() && input_->PushKey(DIK_LSHIFT)) {
			Vector2 delta = input_->GetMouseDelta();
			float panSpeed = 0.01f * moveSpeedMultiplier;
			
			// カメラの右方向と上方向を計算
			float cosH = std::cos(horizontalAngle_);
			float sinH = std::sin(horizontalAngle_);
			
			Vector3 right = { cosH, 0.0f, -sinH };
			Vector3 up = { 0.0f, 1.0f, 0.0f };
			
			// 注視点を移動
			targetPosition_.x -= right.x * delta.x * panSpeed;
			targetPosition_.z -= right.z * delta.x * panSpeed;
			targetPosition_.y += up.y * delta.y * panSpeed;
		}

		// ズーム: マウスホイール
		float wheel = input_->GetWheelDelta();
		if (wheel != 0.0f) {
			distance_ -= wheel * scrollSpeed * moveSpeedMultiplier;
			// 距離を制限
			if (distance_ < 1.0f) distance_ = 1.0f;
			if (distance_ > 1000.0f) distance_ = 1000.0f;
		}

		UpdateCameraPositionOrbit();
		
	} else {
		// 自由回転モード
		
		// 回転: 中ボタンドラッグ
		if (input_->PushMouseButtonM() && !input_->PushKey(DIK_LSHIFT)) {
			Vector2 delta = input_->GetMouseDelta();
			float rotateSpeed = 0.005f;
			transform_.rotate.y -= delta.x * rotateSpeed * rotateSpeedMultiplier;
			transform_.rotate.x -= delta.y * rotateSpeed * rotateSpeedMultiplier;
			
			// X回転を制限
			const float maxRotX = 1.5f;
			const float minRotX = -1.5f;
			if (transform_.rotate.x > maxRotX) transform_.rotate.x = maxRotX;
			if (transform_.rotate.x < minRotX) transform_.rotate.x = minRotX;
		}

		// 平行移動 (Pan): Shift + 中ボタンドラッグ
		if (input_->PushMouseButtonM() && input_->PushKey(DIK_LSHIFT)) {
			Vector2 delta = input_->GetMouseDelta();
			float panSpeed = 0.05f * moveSpeedMultiplier;
			
			// カメラのローカル座標系での移動
			float cosY = std::cos(transform_.rotate.y);
			float sinY = std::sin(transform_.rotate.y);
			
			Vector3 right = { cosY, 0.0f, -sinY };
			Vector3 up = { 0.0f, 1.0f, 0.0f };
			
			transform_.translate.x -= right.x * delta.x * panSpeed;
			transform_.translate.z -= right.z * delta.x * panSpeed;
			transform_.translate.y += up.y * delta.y * panSpeed;
		}

		// 前後移動: マウスホイール
		float wheel = input_->GetWheelDelta();
		if (wheel != 0.0f) {
			float moveSpeed = scrollSpeed * moveSpeedMultiplier;
			
			// カメラの前方向に移動
			float cosY = std::cos(transform_.rotate.y);
			float sinY = std::sin(transform_.rotate.y);
			float cosX = std::cos(transform_.rotate.x);
			
			Vector3 forward = { sinY * cosX, -std::sin(transform_.rotate.x), cosY * cosX };
			
			transform_.translate.x += forward.x * wheel * moveSpeed;
			transform_.translate.y += forward.y * wheel * moveSpeed;
			transform_.translate.z += forward.z * wheel * moveSpeed;
		}
	}

	// リセット: Rキー
	if (input_->PushKey(DIK_R)) {
		if (cameraMode_ == CameraMode::OrbitAroundOrigin) {
			targetPosition_ = { 0.0f, 0.0f, 0.0f };
			distance_ = 30.0f;
			horizontalAngle_ = 3.14159f;
			verticalAngle_ = 0.0f;
			UpdateCameraPositionOrbit();
		} else {
			transform_.translate = { 0.0f, 2.0f, -30.0f };
			transform_.rotate = { 0.0f, 0.0f, 0.0f };
		}
	}

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
}

void DebugCamera::ToggleCameraMode() {
	if (cameraMode_ == CameraMode::OrbitAroundOrigin) {
		cameraMode_ = CameraMode::FreeRotation;
		// 現在の位置と回転を保持
	} else {
		cameraMode_ = CameraMode::OrbitAroundOrigin;
		// 現在の位置から原点注視モードのパラメータを計算
		Vector3 toCamera = {
			transform_.translate.x - targetPosition_.x,
			transform_.translate.y - targetPosition_.y,
			transform_.translate.z - targetPosition_.z
		};
		distance_ = std::sqrt(toCamera.x * toCamera.x + toCamera.y * toCamera.y + toCamera.z * toCamera.z);
		horizontalAngle_ = std::atan2(toCamera.x, toCamera.z);
		verticalAngle_ = std::asin(toCamera.y / distance_);
	}
}

void DebugCamera::UpdateCameraPositionOrbit() {
	// 球面座標系でカメラ位置を計算
	float cosVertical = std::cos(verticalAngle_);
	float sinVertical = std::sin(verticalAngle_);
	float cosHorizontal = std::cos(horizontalAngle_);
	float sinHorizontal = std::sin(horizontalAngle_);
	
	// 注視点からの相対位置
	Vector3 offset;
	offset.x = distance_ * cosVertical * sinHorizontal;
	offset.y = distance_ * sinVertical;
	offset.z = distance_ * cosVertical * cosHorizontal;
	
	// カメラの位置
	transform_.translate = {
		targetPosition_.x + offset.x,
		targetPosition_.y + offset.y,
		targetPosition_.z + offset.z
	};
	
	// カメラから注視点への方向ベクトルを計算
	Vector3 forward = {
		targetPosition_.x - transform_.translate.x,
		targetPosition_.y - transform_.translate.y,
		targetPosition_.z - transform_.translate.z
	};
	
	// 正規化
	float length = std::sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
	if (length > 0.0f) {
		forward.x /= length;
		forward.y /= length;
		forward.z /= length;
	}

	// ピッチ（X軸回転）を計算
	float pitch = std::asin(-forward.y);
	
	// ヨー（Y軸回転）を計算
	float yaw = std::atan2(forward.x, forward.z);

	// 回転を設定
	transform_.rotate = { pitch, yaw, 0.0f };
}

void DebugCamera::DrawImgui() {
#ifdef USE_IMGUI
	ImGui::Begin("デバッグカメラ操作");

	ImGui::Text("操作方法:");
	ImGui::Text("- モード切り替え: Tキー");
	ImGui::Text("- 回転: 中ボタン押しながらドラッグ");
	ImGui::Text("- 平行移動(パン): Shift + 中ボタン押しながらドラッグ");
	ImGui::Text("- ズーム/移動: マウスホイール");
	ImGui::Text("- リセット: Rキー");

	ImGui::Separator();

	// モード選択
	const char* modeNames[] = { "原点の周りを周回", "自由回転" };
	int currentMode = static_cast<int>(cameraMode_);
	if (ImGui::Combo("カメラモード", &currentMode, modeNames, 2)) {
		cameraMode_ = static_cast<CameraMode>(currentMode);
		if (cameraMode_ == CameraMode::OrbitAroundOrigin) {
			// 原点注視モードに切り替わった時、パラメータを再計算
			Vector3 toCamera = {
				transform_.translate.x - targetPosition_.x,
				transform_.translate.y - targetPosition_.y,
				transform_.translate.z - targetPosition_.z
			};
			distance_ = std::sqrt(toCamera.x * toCamera.x + toCamera.y * toCamera.y + toCamera.z * toCamera.z);
			if (distance_ > 0.0f) {
				horizontalAngle_ = std::atan2(toCamera.x, toCamera.z);
				verticalAngle_ = std::asin(toCamera.y / distance_);
			}
		}
	}

	ImGui::Separator();

	if (cameraMode_ == CameraMode::OrbitAroundOrigin) {
		ImGui::Text("=== 周回モード ===");
		ImGui::DragFloat3("注視点の位置", &targetPosition_.x, 0.1f);
		ImGui::DragFloat("距離", &distance_, 0.1f, 1.0f, 100.0f);
		ImGui::DragFloat("水平角", &horizontalAngle_, 0.01f);
		ImGui::DragFloat("垂直角", &verticalAngle_, 0.01f, -1.5f, 1.5f);
	} else {
		ImGui::Text("=== 自由モード ===");
	}

	ImGui::Separator();

	ImGui::DragFloat3("カメラ位置", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("カメラ回転", &transform_.rotate.x, 0.01f);

	ImGui::Separator();

	ImGui::DragFloat("移動速度倍率", &moveSpeedMultiplier, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("回転速度倍率", &rotateSpeedMultiplier, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("スクロール速度", &scrollSpeed, 0.1f, 0.1f, 10.0f);
	if (ImGui::Button("上記の3点の値を保存")) {
		Save();
	}

	if (ImGui::Button("リセット")) {
		if (cameraMode_ == CameraMode::OrbitAroundOrigin) {
			targetPosition_ = { 0.0f, 0.0f, 0.0f };
			distance_ = 30.0f;
			horizontalAngle_ = 3.14159f;
			verticalAngle_ = 0.0f;
		} else {
			transform_.translate = { 0.0f, 2.0f, -30.0f };
			transform_.rotate = { 0.0f, 0.0f, 0.0f };
		}
	}

	ImGui::End();

#endif
}

void DebugCamera::Save() {
	binaryManager_->RegistOutput(scrollSpeed);
	binaryManager_->RegistOutput(moveSpeedMultiplier);
	binaryManager_->RegistOutput(rotateSpeedMultiplier);

	binaryManager_->Write(binaryFileName_);
}