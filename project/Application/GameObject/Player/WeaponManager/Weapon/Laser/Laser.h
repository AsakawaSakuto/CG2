#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Laser : public BaseGameObject {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Laser() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }
    void SetDirectionToEnemy(const Vector3& direction) { directionToEnemy_ = direction.Normalized(); }

	void SetPenetrationCount(int count) { penetrationCount_ = count; }
    void DecrementPenetrationCount() {
        if (penetrationCount_ > 0) {
            penetrationCount_--;
        }
    }
	int GetPenetrationCount() const { return penetrationCount_; }
private:
    unique_ptr<Particles> particle_ = make_unique<Particles>();
    unique_ptr<Particles> particle2_ = make_unique<Particles>();
    Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
    GameTimer lifeTimer_;
    int penetrationCount_ = 0;
    int bounceCount_ = 0;

    float speed_ = 15.0f;
};