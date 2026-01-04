#pragma once
#include "GameObject/Player/WeaponManager/WeaponStatus.h"
#include "Camera/Camera.h"
#include <queue>

#include"GameObject/Rarity.h"

#include "FireBall/FireBall.h"
#include "Laser/Laser.h"
#include "Runa/Runa.h"
#include "Axe/Axe.h"
#include "Boomerang/Boomerang.h"
#include "Dice/Dice.h"
#include "Toxic/Toxic.h"
#include "Area/Area.h"
#include "Gun/Gun.h"

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
	void SetRandomDirectionToEnemy(const Vector3& direction) { randomDirectionToEnemy_ = direction; }

	// 弾のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<FireBall>>& GetFireBalls() const { return fireBall_; }
	const std::vector<std::unique_ptr<Laser>>& GetLaser() const { return laser_; }
	const std::vector<std::unique_ptr<Runa>>& GetRuna() const { return runa_; }
	const std::vector<std::unique_ptr<Axe>>& GetAxe() const { return axe_; }
	const std::vector<std::unique_ptr<Boomerang>>& GetBoomerang() const { return boomerang_; }
	const std::vector<std::unique_ptr<Dice>>& GetDice() const { return dice_; }
	const std::vector<std::unique_ptr<Toxic>>& GetToxic() const { return toxic_; }
	const std::unique_ptr<Area>& GetArea() const { return area_; }
	const std::vector<std::unique_ptr<Gun>>& GetGun() const { return gun_; }

	void PostFrameCleanup();
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="weapon"></param>
	void SetWeaponName(WeaponName weapon);
	
	/// <summary>
	/// 武器の名前を取得
	/// </summary>
	WeaponName GetWeaponName() const { return weaponName_; }
	
	/// <summary>
	/// 敵に与えるダメージを強化
	/// </summary>
	/// <param name="rarity"></param>
	void UpgradeDamage(Rarity rarity) { 
		switch (rarity)
		{
		case Rarity::UnCommon:
			status_.damage += 1.0f;
			break;
		case Rarity::Rare:
			status_.damage += 1.5f;
			break;
		case Rarity::Epic:
			status_.damage += 2.0f;
			break;
		case Rarity::Legendary:
			status_.damage += 3.0f;
			break;
		}
	}

	/// <summary>
	/// 最大発射数を強化
	/// </summary>
	/// <param name="rarity"></param>
	void UpgradeShotMaxCount(Rarity rarity) {
		switch (rarity)
		{
		case Rarity::UnCommon:
			status_.shotMaxCount += 1.0f;
			break;
		case Rarity::Rare:
			status_.shotMaxCount += 1.25f;
			break;
		case Rarity::Epic:
			status_.shotMaxCount += 1.5f;
			break;
		case Rarity::Legendary:
			status_.shotMaxCount += 2.0f;
			break;
		}
	}

	/// <summary>
	/// サイズを強化（AreaとToxic専用）
	/// </summary>
	/// <param name="rarity"></param>
	void UpgradeSize(Rarity rarity) {
		switch (rarity)
		{
		case Rarity::UnCommon:
			status_.sizeRate += 0.05f;
			break;
		case Rarity::Rare:
			status_.sizeRate += 0.1f;
			break;
		case Rarity::Epic:
			status_.sizeRate += 0.15f;
			break;
		case Rarity::Legendary:
			status_.sizeRate += 0.25f;
			break;
		}
	}
private:

	void FireBallUpdate();
	void LaserUpdate();
	void RunaUpdate();
	void AxeUpdate();
	void BoomerangUpdate();
	void DiceUpdate();
	void ToxicUpdate();
	void AreaUpdate();
	void GunUpdate();

private:

	std::vector<std::unique_ptr<FireBall>> fireBall_;
	std::vector<std::unique_ptr<Laser>> laser_;
	std::vector<std::unique_ptr<Runa>> runa_;
	std::vector<std::unique_ptr<Axe>> axe_;
	std::vector<std::unique_ptr<Boomerang>> boomerang_;
	std::vector<std::unique_ptr<Dice>> dice_;
	std::vector<std::unique_ptr<Toxic>> toxic_;
	std::unique_ptr<Area> area_;  // Areaは単一インスタンス
	std::vector<std::unique_ptr<Gun>> gun_;


	Vector3 spawnOffSet_ = { 0.0f, 1.0f, 0.0f };

	WeaponName weaponName_;
	WeaponStatus status_;

	GameTimer coolDownTimer_; // クールタイムタイマー
	GameTimer intervalTimer_; // 攻撃間隔タイマー

	Vector3 playerPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
	Vector3 randomDirectionToEnemy_ = { 0.0f, 0.0f, 0.0f };
	
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