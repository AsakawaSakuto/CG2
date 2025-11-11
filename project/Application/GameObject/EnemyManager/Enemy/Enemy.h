#pragma once
#include "Application/GameObject/BaseGameObject.h"
#include "Application/EngineSystem.h"

class Enemy : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

private:
    unique_ptr<Model> model_ = make_unique<Model>();
};