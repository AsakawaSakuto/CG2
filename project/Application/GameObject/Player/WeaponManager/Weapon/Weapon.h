#pragma once
#include "Application/AppContext.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Bullet/bullet.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Area/area.h"
#include "Application/GameObject/Player/WeaponManager/WeaponStatus.h"
#include "Application/GameObject/Player/WeaponManager/WeaponName.h"
#include "Camera.h"
#include <queue>

/// <summary>
/// Update関数をラムダ式にして、Enumでそれに応じたUpdate関数を持ってくるやり方を冬休みにやる
/// </summary>
class Weapon {
public:
	void Initialize(AppContext* ctx, WeaponName weaponName);

	void Update();

	void Draw(Camera camera);

	void SetPlayerPosition(const Vector3& position);
	void SetDirectionToEnemy(const Vector3& direction);

	// 弾のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<Bullet>>& GetBullets() const { return bullets_; }

	void PostFrameCleanup();
	
private:

	void BulletTypeUpdate();

	void AreaTypeUpdate();

	void DirectTypeUpdate();
private:
	AppContext* ctx_;

	std::vector<std::unique_ptr<Bullet>> bullets_;
	std::vector<std::unique_ptr<Area>> areas_;

	WeaponName weaponName_;
	WeaponStatus status_;

	EmitterState emitterState1_;
	EmitterState emitterState2_;

	GameTimer coolDownTimer_; // クールタイムタイマー
	GameTimer intervalTimer_; // 攻撃間隔タイマー

	Vector3 playerPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
};