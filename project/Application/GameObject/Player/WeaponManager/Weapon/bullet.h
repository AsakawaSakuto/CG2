#pragma once
#include "Application/GameObject/BaseGameObject.h"
#include "Application/EngineSystem.h"
#include "Application/GameObject/Player/WeaponManager/WeaponStatus.h"

class Weapon : public BaseGameObject {
public:
	void Initialize(AppContext* ctx) override;
	void Update() override;
	void Draw(Camera camera) override;
	void DrawImGui() override;

private:

private:
	unique_ptr<Model> model_ = make_unique<Model>();
	WeaponStatus status_;
};