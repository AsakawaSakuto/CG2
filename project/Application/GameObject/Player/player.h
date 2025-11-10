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
	// ジャンプ処理
	void Jump();

private:
	unique_ptr<Model> model_ = make_unique<Model>();
	
	// 移動関連のメンバ変数
	float moveSpeed_ = 5.0f;
	Camera camera_; // 現在のカメラへの参照
	
	// ジャンプ関連のメンバ変数
	int jumpCanCount_ = 1;           // ジャンプ可能回数
	int currentJumpCount_ = 0;        // 現在のジャンプ回数
	float jumpPower_ = 8.0f;          // ジャンプ力
	float velocity_Y_ = 0.0f;         // Y軸方向の速度
	float gravity_ = 20.0f;           // 重力
	float groundLevel_ = 0.0f;        // 地面のY座標
	bool isGrounded_ = true;          // 地面にいるかどうか
};