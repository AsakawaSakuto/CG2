#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Axe : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Axe() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }
    void SetDirectionToEnemy(const Vector3& direction) { directionToEnemy_ = direction; }
	void SetLifeTime(float lifeTime) { lifeTime_ = lifeTime; }

private:
	unique_ptr<Model> model_ = make_unique<Model>();
    Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
    GameTimer lifeTimer_;
	GameTimer moveTimer_;
	GameTimer stopTimer_;
	GameTimer scaleTimer_;
	float lifeTime_ = 3.0f;

    float speed_ = 5.0f;
};