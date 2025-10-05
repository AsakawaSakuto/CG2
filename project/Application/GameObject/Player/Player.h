#pragma once
#include "Application/GameObject/GameObject.h"

class Player : public GameObject
{
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

private:
	Input* input_ = nullptr;

	Vector3 acceleration_{};
	Vector3 velocity_{};
};