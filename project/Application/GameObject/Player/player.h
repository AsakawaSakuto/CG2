#pragma once
#include "Application/GameObject/BaseGameObject.h"
#include "Application/EngineSystem.h"
#include "playerStatus.h"

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
	unique_ptr<Particles> moveParticle_ = make_unique<Particles>();
	unique_ptr<Particles> landingParticle_ = make_unique<Particles>();

	Camera camera_; // 現在のカメラへの参照
	
	PlayerStatus status_; // プレイヤーステータス

	float collisionRadius_ = 0.5f;

	// ジャンプ関連のメンバ変数
	float groundLevel_ = 0.0f; // 地面のY座標
	bool isGrounded_ = true;   // 地面にいるかどうか
	bool wasGrounded_ = true;  // 前フレームで地面にいたかどうか
};