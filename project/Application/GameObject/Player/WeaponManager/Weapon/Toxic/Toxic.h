#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Toxic : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Toxic() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

	void SetScaleMultipler(float scale) { scaleMultipler_ = scale; }
private:
    unique_ptr<Model> model_ = make_unique<Model>();
	unique_ptr<Particles> toxicParticle_ = make_unique<Particles>();
    GameTimer lifeTimer_;
    GameTimer scaleTimer_;
    float lifeTime_ = 5.0f;
	float scaleMultipler_ = 1.0f;
};