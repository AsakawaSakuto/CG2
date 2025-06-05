#include "Camera.h"
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"

Camera::Camera() {
	transform_ = { {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,-5.f} };
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, 1280.f / 720.f, 0.1f, 100.f);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
}

void Camera::Update() {

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
}
