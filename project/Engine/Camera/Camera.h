#pragma once
#define NOMINMAX
#include "Transform.h"
#include "Matrix4x4.h"
#include "MatrixFunction.h"
#include "Vector3.h"
#include "Engine/System/Utility/Math/Plane.h"

#include "WinApp.h"

// フラスタム構造体
struct Frustum {
	Plane planes[6]; // 6つの平面（左、右、上、下、近、遠）
};

class Camera {
public:
	Camera();

	virtual void Update();

	Matrix4x4 GetVeiwProjectionMatrix() const { return viewProjectionMatrix_; }

	void SetPosition(const Vector3& position) { transform_.translate = position; }

	void DrawImgui();

	void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }

	Vector3& GetScale() { return transform_.scale; }
	Vector3& GetRotate() { return transform_.rotate; }
	Vector3& GetTranslate() { return transform_.translate; }
	Vector3 GetWorldPosition();

	Transform GetTransform() { return transform_; }

	Matrix4x4 GetWorldMatrix() { return worldMatrix_; }
	Matrix4x4 GetViewMatrix() { return viewMatrix_; }
	Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }

	float GetNearClip() { return nearClip_; }
	float GetFarClip() { return farClip_; }
	float GetFovY() { return fovY_; }

	// フラスタムカリング関連
	Frustum GetFrustum() const { return frustum_; }
	bool IsInFrustum(const Vector3& position, float radius) const;

protected:
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewMatrix_;
	Transform transform_;

	float nearClip_ = 0.1f;
	float farClip_ = 1000.0f;
	float fovY_ = 0.45f;

	// フラスタム
	Frustum frustum_;

private:
	// フラスタムを更新する
	void UpdateFrustum();
};

