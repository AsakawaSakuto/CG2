#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Gun : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Gun() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

private:
    unique_ptr<Model> model_;
    unique_ptr<Particles> particle_ = make_unique<Particles>();
    GameTimer lifeTimer_;

    float speed_ = 20.0f;
};