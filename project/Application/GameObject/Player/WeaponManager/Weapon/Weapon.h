#pragma once
#include "Application/AppContext.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Bullet/bullet.h"
#include "Application/GameObject/Player/WeaponManager/WeaponStatus.h"
#include "Camera.h"

class Weapon {
public:
	void Initialize(AppContext* ctx);

	void Update();

	void Draw(Camera camera);
private:
	AppContext* ctx_;
	std::vector<std::unique_ptr<Bullet>> bullets_;

	WeaponStatus status_;
	GameTimer coolDownTimer_; // クールタイムタイマー
	GameTimer intervalTimer_; // 攻撃間隔タイマー
};