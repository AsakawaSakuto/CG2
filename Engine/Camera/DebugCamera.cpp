#include "DebugCamera.h"
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"

DebugCamera::DebugCamera() {
	transform_ = { {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,0.f,-10.f} };
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, 1280.f / 720.f,0.1f, 100.f);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
}

void DebugCamera::Update() {

	if (input_->TriggerMouseButtonR()) {
		if (moveDirection == MOVE_X ) {
			moveDirection = MODE_Y;
		} else if(moveDirection == MODE_Y) {
			moveDirection = MOVE_Z;
		} else if (moveDirection == MOVE_Z) {
			moveDirection = MOVE_X;
		}
	}

	if (input_->PushKey(DIK_A)) {
		transform_.translate.x -= 0.05f;
	}
	if (input_->PushKey(DIK_D)) {
		transform_.translate.x += 0.05f;
	}
	if (input_->PushKey(DIK_W)) {
		transform_.translate.y += 0.05f;
	}
	if (input_->PushKey(DIK_S)) {
		transform_.translate.y -= 0.05f;
	}

	wheel = input_->GetWheelDelta();
	scrollSpeed = 0.5f;

	switch (moveDirection)
	{
	case MOVE_X:
		transform_.translate.x += wheel * scrollSpeed;
		break;
	case MODE_Y:
		transform_.translate.y += wheel * scrollSpeed;
		break;
	case MOVE_Z:
		transform_.translate.z += wheel * scrollSpeed;
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
	
	const char* directionLabels[] = { "X", "Y", "Z" };
    int current = static_cast<int>(moveDirection);
    ImGui::Combo("Move Axis", &current, directionLabels, 3);
    moveDirection = static_cast<MoveDirection>(current);

	ImGui::End();
}