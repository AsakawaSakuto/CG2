#include "DebugCamera.h"
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"

DebugCamera::DebugCamera() {
	moveSpeedMultiplier = 1.0f;
	rotateSpeedMultiplier = 1.0f;
	transform_ = { {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,2.f,-30.f} };
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, 1280.f / 720.f,0.1f, 100.f);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
}

void DebugCamera::Update() {

	/*if (input_->TriggerMouseButtonR()) {
		if (moveDirection == MOVE_X ) {
			moveDirection = MODE_Y;
		} else if(moveDirection == MODE_Y) {
			moveDirection = MOVE_Z;
		} else if (moveDirection == MOVE_Z) {
			moveDirection = MOVE_X;
		}
	}*/

	if (!ImGui::GetIO().WantCaptureMouse && input_->PushMouseButtonL()) {
		Vector2 delta = input_->GetMouseDelta();
		float rotateSpeed = 0.001f;
		transform_.rotate.y += delta.x * rotateSpeed * rotateSpeedMultiplier;
		transform_.rotate.x += delta.y * rotateSpeed * rotateSpeedMultiplier;
	}

	if (input_->PushKey(DIK_R)) {
		transform_.rotate.x = 0.0f;
		transform_.rotate.y = 0.0f;
		transform_.rotate.z = 0.0f;
	}
	if (input_->PushKey(DIK_T)) {
		transform_.translate.x = 0.0f;
		transform_.translate.y = 0.0f;
		transform_.translate.z = -10.0f;
	}

	if (input_->PushKey(DIK_A)) {
		transform_.translate.x -= 0.05f * moveSpeedMultiplier;
	}
	if (input_->PushKey(DIK_D)) {
		transform_.translate.x += 0.05f * moveSpeedMultiplier;
	}
	if (input_->PushKey(DIK_W)) {
		transform_.translate.y += 0.05f * moveSpeedMultiplier;
	}
	if (input_->PushKey(DIK_S)) {
		transform_.translate.y -= 0.05f * moveSpeedMultiplier;
	}

	wheel = input_->GetWheelDelta();
	scrollSpeed = 0.5f * moveSpeedMultiplier;

	switch (moveDirection)
	{
	case MOVE_X:
		transform_.translate.x += wheel * scrollSpeed * moveSpeedMultiplier;
		break;
	case MODE_Y:
		transform_.translate.y += wheel * scrollSpeed * moveSpeedMultiplier;
		break;
	case MOVE_Z:
		transform_.translate.z += wheel * scrollSpeed * moveSpeedMultiplier;
		break;
	}

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
}

void DebugCamera::DrawImgui() {

	ImGui::Begin("DebugCamera Control");

	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);

	ImGui::DragFloat("MoveMultiplier", &moveSpeedMultiplier, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("RotateMultiplier", &rotateSpeedMultiplier, 0.01f, 0.0f, 10.0f);

	if (ImGui::Button("Reset")) {
		transform_.translate = { 0.0f,0.0f,-10.0f };
		transform_.rotate = { 0.0f,0.0f,0.0f };
	}

	ImGui::End();
}