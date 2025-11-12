#pragma once
#include "Application/GameObject/BaseGameObject.h"
#include "Application/EngineSystem.h"

class Enemy : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

	void SetTargetPosition(const Vector3& target) { targetPosition_ = target; }

private:

    void Move();

private:
    unique_ptr<Model> model_ = make_unique<Model>();
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };
	float moveSpeed_ = 2.0f; // デフォルトの移動速度
};