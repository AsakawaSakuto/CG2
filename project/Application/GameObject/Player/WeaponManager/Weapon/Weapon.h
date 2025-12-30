#pragma once
#include "GameObject/Player/WeaponManager/Weapon/Bullet/bullet.h"
#include "GameObject/Player/WeaponManager/WeaponStatus.h"
#include "Camera/Camera.h"
#include <queue>

#include "FireBall/FireBall.h"
#include "Laser/Laser.h"
#include "Runa/Runa.h"

/// <summary>
/// Update関数をラムダ式にして、Enumでそれに応じたUpdate関数を持ってくるやり方を冬休みにやる
/// </summary>
class Weapon {
public:
	void Initialize(WeaponName weaponName);

	void Update();

	void Draw(Camera camera);

	void SetPlayerPosition(const Vector3& position) { playerPosition_ = position; }
	void SetDirectionToEnemy(const Vector3& direction) { directionToEnemy_ = direction; }

	// 弾のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<FireBall>>& GetFireBalls() const { return fireBall_; }

	void PostFrameCleanup();
	
private:

	void FireBallUpdate();

private:

	std::vector<std::unique_ptr<FireBall>> fireBall_;
	Vector3 spawnOffSet_ = { 0.0f, 1.0f, 0.0f };

	WeaponName weaponName_;
	WeaponStatus status_;

	GameTimer coolDownTimer_; // クールタイムタイマー
	GameTimer intervalTimer_; // 攻撃間隔タイマー

	Vector3 playerPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
};