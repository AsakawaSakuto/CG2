#pragma once
#include "Application/GameObject/BaseGameObject.h"
#include "Application/EngineSystem.h"

class Player : public BaseGameObject {
public:
    void Initialize(AppContext* ctx) override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    const Vector3& GetPosition() { return transform_.translate; }
private:
	unique_ptr<Model> model_ = make_unique<Model>();
};