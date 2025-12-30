#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class FireBall : public BaseGameObject {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    // デストラクタを追加（unique_ptrの確実な解放を保証）
    ~FireBall() override = default;

    void SetPosition(const Vector3& position);
    void SetDirectionToEnemy(const Vector3& direction);

private:
    unique_ptr<Particles> particle_ = make_unique<Particles>();
    unique_ptr<Particles> particle2_ = make_unique<Particles>();
    Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
    GameTimer lifeTimer_;
    int penetrationCount_ = 0;
    int bounceCount_ = 0;

    float speed_ = 10.0f;
};