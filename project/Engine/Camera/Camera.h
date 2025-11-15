#pragma once
#define NOMINMAX
#include "Transform.h"
#include "Matrix4x4.h"
#include "MatrixFunction.h"
#include "Vector3.h"
#include "Engine/System/Utility/Math/Plane.h"

#include "WinApp.h"
#include <cmath>
#include <algorithm>

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

	// TPS機能
	void SetTarget(const Vector3& target) { target_ = target; }
	void SetDistance(float distance) { distance_ = distance; }
	void SetTPSAngles(float horizontal, float vertical) { 
		horizontalAngle_ = horizontal; 
		verticalAngle_ = vertical; 
	}

	void AddTPSAngles(float deltaHorizontal, float deltaVertical) {
		horizontalAngle_ += deltaHorizontal;
		
		// 仮の垂直角度を計算
		float newVerticalAngle = verticalAngle_ + deltaVertical;
		
		// 垂直角度を制限（真上と真下を避ける）
		const float maxVertical = 1.5f;
		const float minVertical = -1.5f;
		if (newVerticalAngle > maxVertical) newVerticalAngle = maxVertical;
		if (newVerticalAngle < minVertical) newVerticalAngle = minVertical;
		
		// 地面貫通チェック：新しい角度でカメラ位置を計算
		const float minHeight = 0.1f;
		float testY = target_.y + distance_ * std::sin(newVerticalAngle);
		
		// カメラが地面より下にならない角度を計算
		if (testY < minHeight) {
			// 地面ギリギリの角度を計算
			float maxAllowedSin = (minHeight - target_.y) / distance_;
			// sinの値を-1.0～1.0の範囲に制限
			maxAllowedSin = (std::max)(-1.0f, std::min(1.0f, maxAllowedSin));
			float maxAllowedAngle = std::asin(maxAllowedSin);
			
			// 現在の角度が既に制限値に達している場合、下方向への変更を無視
			if (deltaVertical < 0.0f) {
				verticalAngle_ = maxAllowedAngle;
			} else {
				verticalAngle_ = newVerticalAngle;
			}
		} else {
			verticalAngle_ = newVerticalAngle;
		}
	}
	
	Vector3 GetTarget() const { return target_; }
	float GetDistance() const { return distance_; }
	float GetHorizontalAngle() const { return horizontalAngle_; }
	float GetVerticalAngle() const { return verticalAngle_; }

	// TPSモードでの更新
	void UpdateTPS();

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

	// TPS用パラメータ
	Vector3 target_ = {0.0f, 0.0f, 0.0f};      // 注視点
	float distance_ = 10.0f;                   // ターゲットからの距離
	float horizontalAngle_ = 0.0f;             // 水平角度（Y軸周り）
	float verticalAngle_ = 0.0f;               // 垂直角度（X軸周り相当）

private:
	// フラスタムを更新する
	void UpdateFrustum();
	
	// TPSカメラの位置を計算
	Vector3 CalculateTPSPosition() const;
};