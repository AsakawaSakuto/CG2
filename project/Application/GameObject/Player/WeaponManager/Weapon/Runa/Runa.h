#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Runa : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Runa() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }
    void SetDirectionToEnemy(const Vector3& direction) { directionToEnemy_ = direction; }

    void Bounce() {
        bounceCount_--;
        if (bounceCount_ > 0) {
			int r = MyRand::Int(0, 2);
            if (r == 0) { directionToEnemy_.x *= -1.0f; }
            if (r == 1) { directionToEnemy_.z *= -1.0f; }
            if (r == 2) { directionToEnemy_.x *= -1.0f; directionToEnemy_.z *= -1.0f; }
        } else {
			Dead();
        }
    }
private:
    unique_ptr<Particles> particle_ = make_unique<Particles>();
    unique_ptr<Particles> particle2_ = make_unique<Particles>();
    Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
    GameTimer lifeTimer_;

    float speed_ = 10.0f;
};