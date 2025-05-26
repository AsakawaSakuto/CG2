#pragma once
#define NOMINMAX
#include "Transform.h"
#include "Matrix4x4.h"


class Camera {
public:
	Camera();

	virtual void Update();

	Matrix4x4 GetVeiwProjectionMatrix() const { return viewProjectionMatrix_; }

	void SetPosition(const Vector3& position) { transform_.translate = position; }
	Vector3& GetTranslate() { return transform_.translate; }
	Vector3& GetRotate() { return transform_.rotate; }
	Transform GetTransform()const { return transform_; }
protected:
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewProjectionMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewMatrix_;
	Transform transform_;
};

