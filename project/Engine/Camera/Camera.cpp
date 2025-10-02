#include "Camera.h"
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"

Camera::Camera() {
	transform_ = { {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,4.f,-29.f} };
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), nearClip_, farClip_);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
}

void Camera::Update() {

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
}

void Camera::DrawImgui() {

	ImGui::Begin("Camera Control");

	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
}

Vector3 Camera::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldMatrix_.m[3][0];
	worldPos.y = worldMatrix_.m[3][1];
	worldPos.z = worldMatrix_.m[3][2];

	return worldPos;
}