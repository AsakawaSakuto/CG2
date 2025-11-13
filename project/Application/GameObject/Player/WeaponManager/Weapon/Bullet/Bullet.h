#pragma once
#include "Application/EngineSystem.h"
#include "Application/GameObject/BaseGameObject.h"

class Bullet : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

private:

private:
    unique_ptr<Model> model_ = make_unique<Model>();
};