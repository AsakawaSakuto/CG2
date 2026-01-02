#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Boomerang : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Boomerang() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

private:
    unique_ptr<Model> model_ = make_unique<Model>();
    unique_ptr<Particles> trailParticle_ = make_unique<Particles>();
    unique_ptr<Particles> trail2Particle_ = make_unique<Particles>();
    GameTimer lifeTimer_;
	GameTimer scaleTimer_;
    GameTimer goTimer_;
    GameTimer backTimer_;
    float lifeTime_ = 20.0f;

    float speed_ = 15.0f;
};