#include "Camera.h"
#include "externals/imgui/imgui.h"           
#include "externals/imgui/imgui_impl_dx12.h" 
#include "externals/imgui/imgui_impl_win32.h"
#include <cmath>

Camera::Camera() {
	transform_ = { {1.f,1.f,1.f},{0.f,0.f,0.f},{0.f,4.f,-29.f} };
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), nearClip_, farClip_);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
	
	// フラスタムを初期化
	UpdateFrustum();
}

void Camera::Update() {
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = InverseMatrix(worldMatrix_);
	viewProjectionMatrix_ = MultiplyMatrix(viewMatrix_, projectionMatrix_);
	
	// フラスタムを更新
	UpdateFrustum();
}

void Camera::UpdateFrustum() {
	Matrix4x4 viewProjection = viewProjectionMatrix_;
	
	// フラスタム平面を抽出
	// Left plane
	frustum_.planes[0].normal.x = viewProjection.m[0][3] + viewProjection.m[0][0];
	frustum_.planes[0].normal.y = viewProjection.m[1][3] + viewProjection.m[1][0];
	frustum_.planes[0].normal.z = viewProjection.m[2][3] + viewProjection.m[2][0];
	frustum_.planes[0].distance = viewProjection.m[3][3] + viewProjection.m[3][0];
	
	// Right plane
	frustum_.planes[1].normal.x = viewProjection.m[0][3] - viewProjection.m[0][0];
	frustum_.planes[1].normal.y = viewProjection.m[1][3] - viewProjection.m[1][0];
	frustum_.planes[1].normal.z = viewProjection.m[2][3] - viewProjection.m[2][0];
	frustum_.planes[1].distance = viewProjection.m[3][3] - viewProjection.m[3][0];
	
	// Bottom plane
	frustum_.planes[2].normal.x = viewProjection.m[0][3] + viewProjection.m[0][1];
	frustum_.planes[2].normal.y = viewProjection.m[1][3] + viewProjection.m[1][1];
	frustum_.planes[2].normal.z = viewProjection.m[2][3] + viewProjection.m[2][1];
	frustum_.planes[2].distance = viewProjection.m[3][3] + viewProjection.m[3][1];
	
	// Top plane
	frustum_.planes[3].normal.x = viewProjection.m[0][3] - viewProjection.m[0][1];
	frustum_.planes[3].normal.y = viewProjection.m[1][3] - viewProjection.m[1][1];
	frustum_.planes[3].normal.z = viewProjection.m[2][3] - viewProjection.m[2][1];
	frustum_.planes[3].distance = viewProjection.m[3][3] - viewProjection.m[3][1];
	
	// Near plane
	frustum_.planes[4].normal.x = viewProjection.m[0][3] + viewProjection.m[0][2];
	frustum_.planes[4].normal.y = viewProjection.m[1][3] + viewProjection.m[1][2];
	frustum_.planes[4].normal.z = viewProjection.m[2][3] + viewProjection.m[2][2];
	frustum_.planes[4].distance = viewProjection.m[3][3] + viewProjection.m[3][2];
	
	// Far plane
	frustum_.planes[5].normal.x = viewProjection.m[0][3] - viewProjection.m[0][2];
	frustum_.planes[5].normal.y = viewProjection.m[1][3] - viewProjection.m[1][2];
	frustum_.planes[5].normal.z = viewProjection.m[2][3] - viewProjection.m[2][2];
	frustum_.planes[5].distance = viewProjection.m[3][3] - viewProjection.m[3][2];
	
	// 各平面を正規化
	for (int i = 0; i < 6; i++) {
		float length = std::sqrt(
			frustum_.planes[i].normal.x * frustum_.planes[i].normal.x +
			frustum_.planes[i].normal.y * frustum_.planes[i].normal.y +
			frustum_.planes[i].normal.z * frustum_.planes[i].normal.z
		);
		
		if (length > 0.0f) {
			frustum_.planes[i].normal.x /= length;
			frustum_.planes[i].normal.y /= length;
			frustum_.planes[i].normal.z /= length;
			frustum_.planes[i].distance /= length;
		}
	}
}

bool Camera::IsInFrustum(const Vector3& position, float radius) const {
	// 各フラスタム平面に対してオブジェクトが範囲内にあるかチェック
	for (int i = 0; i < 6; i++) {
		float distance = 
			frustum_.planes[i].normal.x * position.x +
			frustum_.planes[i].normal.y * position.y +
			frustum_.planes[i].normal.z * position.z +
			frustum_.planes[i].distance;
			
		// オブジェクトが平面の外側にある場合（距離がマイナス値で、その絶対値が半径より大きい）
		if (distance < -radius) {
			return false; // フラスタム外
		}
	}
	return true; // フラスタム内
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