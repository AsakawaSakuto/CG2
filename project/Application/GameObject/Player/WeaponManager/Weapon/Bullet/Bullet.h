#pragma once
#include "Application/EngineSystem.h"
#include "Application/GameObject/BaseGameObject.h"

class Bullet : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    void SetPosition(const Vector3& position);
    void SetDirectionToEnemy(const Vector3& direction);

private:

private:
    unique_ptr<Particles> particle_ = make_unique<Particles>();
	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
	GameTimer lifeTimer_;
};