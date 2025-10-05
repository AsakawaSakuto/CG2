#pragma once
#include "Application/GameObject/GameObject.h"

enum class Direction
{
	UP = 0,
	DOWN = 1,
};

class Player : public GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Player() {}

	void SetInputSystem(Input* inInput) { input_ = inInput; }

	// ImGui表示
	void DrawImgui();

	// Getter
	Vector3 GetPosition() const { return transform_.translate; }
	float CameraOffset() const { return cameraOffset_; }

private:
	// プレイヤーの上昇
	void MovePlayerUpward();

	// プレイヤーのVelocityを一定の値に収める
	void ClampPlayerVelocity();

	// プレイヤーが一定の高さに到達したら反転
	void ReverseIfAboveLimit(float minHeight, float maxHeight);

	// オフセットの変更(補間)
	void CameraOffsetChange();

	// プレイヤーの回転
	void RotateChange();

private:
	Input* input_ = nullptr;

	// 速度関連
	Vector3 acceleration_{};
	Vector3 velocity_{};

	// オフセット変更フラグ
	bool isOffsetChange_ = false;

	// プレイヤーとカメラのオフセット
	float cameraOffset_ = 4.0f;

	// プレイヤーの進行方向
	Direction direction_ = Direction::UP;
};