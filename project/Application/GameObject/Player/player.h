#pragma once
#include "Application/GameObject/BaseGameObject.h"
#include "Application/EngineSystem.h"
#include "externals/imgui/imgui.h"

class Player : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    const Vector3& GetPosition() { return transform_.translate; }
    
private:

	// 移動処理
	void Move();
	// カメラの向きに基づいた移動方向を計算
	Vector3 CalculateCameraMoveDirection(float stickX, float stickY);

private:
	unique_ptr<Model> model_ = make_unique<Model>();
	
	// 移動関連のメンバ変数
	float moveSpeed_ = 5.0f;
	Camera camera_; // 現在のカメラへの参照
	
};