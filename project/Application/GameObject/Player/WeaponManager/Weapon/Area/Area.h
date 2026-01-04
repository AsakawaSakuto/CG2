#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Area : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Area() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

    void SetScaleMultipler(float scale) { scaleRate_ = scale; }
private:
    unique_ptr<Model> model_ = make_unique<Model>();
	unique_ptr<Particles> areaParticle_ = make_unique<Particles>();
};