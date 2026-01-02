#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class Dice : public BaseGameObject, public BaseWeapon {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;

    ~Dice() override = default;

    void SetPosition(const Vector3& position) { transform_.translate = position; }

    float GetRandDamage() const {
        // 1から6までのランダムな整数を生成
        return MyRand::Float(damage_, damage_ + 5.0f);
	}
private:
    unique_ptr<Model> model_ = make_unique<Model>();
    GameTimer lifeTimer_;
    GameTimer scaleTimer_;
    float lifeTime_ = 10.0f;

    float speed_ = 10.0f;
};