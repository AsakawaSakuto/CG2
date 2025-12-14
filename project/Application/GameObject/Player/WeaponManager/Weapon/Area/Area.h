#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Area : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Area() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

    void LoadBinary(const std::string& filePath, const std::string& filePath2 = "none") {
        particle_->LoadBinary(filePath);
        particle2_->LoadBinary(filePath2);
    }

private:
    unique_ptr<Model> model_ = make_unique<Model>();
    unique_ptr<Particles> particle_ = make_unique<Particles>();
    unique_ptr<Particles> particle2_ = make_unique<Particles>();

    GameTimer lifeTimer_;
	GameTimer scaleTimer_;
};