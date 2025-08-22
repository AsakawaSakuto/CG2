#pragma once
#define NOMINMAX
#include "Transform.h"
#include "Matrix4x4.h"
#include "MatrixFunction.h"

class Camera {
public:
	Camera();

	virtual void Update();

	Matrix4x4 GetVeiwProjectionMatrix() const { return viewProjectionMatrix_; }

	void SetPosition(const Vector3& position) { transform_.translate = position; }

	void DrawImgui();

	Vector3& GetScale() { return transform_.scale; }
	Vector3& GetRotate() { return transform_.rotate; }
	Vector3& GetTranslate() { return transform_.translate; }
	Vector3 GetWorldPosition();

	Transform GetTransform() { return transform_; }

	Matrix4x4 GetWorldMatrix() { return worldMatrix_; }
	Matrix4x4 GetViewMatrix() { return viewMatrix_; }
	Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; }
protected:
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewMatrix_;
	Transform transform_;
};

