#pragma once
#include "GameObject/Player/WeaponManager/WeaponStatus.h"
#include "Camera/Camera.h"
#include <queue>

#include "FireBall/FireBall.h"
#include "Laser/Laser.h"
#include "Runa/Runa.h"
#include "Axe/Axe.h"

/// <summary>
/// Update関数をラムダ式にして、Enumでそれに応じたUpdate関数を持ってくるやり方を冬休みにやる
/// </summary>
class Weapon {
public:
	void Initialize(WeaponName weaponName);

	void Update();

	void Draw(Camera camera);

	void SetPlayerPosition(const Vector3& position) { playerPosition_ = position; }
	void SetDirectionToEnemy(const Vector3& direction) { directionToEnemy_ = direction.Normalized(); }

	// 弾のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<FireBall>>& GetFireBalls() const { return fireBall_; }
	const std::vector<std::unique_ptr<Laser>>& GetLaser() const { return laser_; }
	const std::vector<std::unique_ptr<Runa>>& GetRuna() const { return runa_; }

	void PostFrameCleanup();
	
private:

	void FireBallUpdate();
	void LaserUpdate();
	void RunaUpdate();
	void AxeUpdate();

private:

	std::vector<std::unique_ptr<FireBall>> fireBall_;
	std::vector<std::unique_ptr<Laser>> laser_;
	std::vector<std::unique_ptr<Runa>> runa_;
	std::vector<std::unique_ptr<Axe>> axe_;

	Vector3 spawnOffSet_ = { 0.0f, 1.0f, 0.0f };

	WeaponName weaponName_;
	WeaponStatus status_;

	GameTimer coolDownTimer_; // クールタイムタイマー
	GameTimer intervalTimer_; // 攻撃間隔タイマー

	Vector3 playerPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };

	template <class T>
	void EraseDead(std::vector<std::unique_ptr<T>>& v) {
		v.erase(
			std::remove_if(v.begin(), v.end(),
				[](const std::unique_ptr<T>& p)
				{
					return !p || !p->IsAlive();
				}),
			v.end()
		);
	}

	template<class T, class... Args>
	void DrawVec(const std::vector<std::unique_ptr<T>>& v, Args&&... args) {
		for (const auto& p : v) {
			if (p) {
				p->Draw(std::forward<Args>(args)...);
			}
		}
	}

};