#pragma once
#include "GameObject/BaseGameObject.h"
#include "EngineSystem.h"

class Enemy : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;
    ~Enemy() override = default; // デストラクタを追加（unique_ptrの確実な解放を保証）

	void SetPosition(const Vector3& position) { transform_.translate = position; }

	void SetTargetPosition(const Vector3& target) { targetPosition_ = target; }
	void PushAway(const Vector3& otherPosition, float otherRadius);
private:

    void Move();

private:
    unique_ptr<Model> model_ = make_unique<Model>();
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };
	float moveSpeed_ = 2.0f; // デフォルトの移動速度
	float collicionRadius_ = 0.5f;

	GameTimer scaleTimer_;
};