#pragma once
#include "GameObject/Player/WeaponManager/WeaponStatus.h"
#include "Camera/Camera.h"
#include <queue>
#include <functional>

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

	// 武器の初期化
	void Initialize(WeaponName weaponName);

	// 武器の更新
	void Update();

	// 武器の描画
	void Draw(Camera camera);

	// プレイヤーの位置と敵への方向を設定
	void SetPlayerPosition(const Vector3& position) { playerPosition_ = position; }

	// 敵への方向を設定
	void SetDirectionToEnemy(const Vector3& direction) { directionToEnemy_ = direction; }

	// 敵へのランダム方向を設定
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
		status_.upgradeCount++;
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
		status_.upgradeCount++;
	}

	/// <summary>
	/// サイズを強化（Area、Toxic、Axe、Boomerang専用）
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
		status_.upgradeCount++;
	}

	/// <summary>
	/// 持続時間を強化（AxeとToxic専用）
	/// </summary>
	/// <param name="rarity"></param>
	void UpgradeLifeTime(Rarity rarity) {
		switch (rarity)
		{
		case Rarity::UnCommon:
			status_.lifeTime += 0.25f;
			break;
		case Rarity::Rare:
			status_.lifeTime += 0.5f;
			break;
		case Rarity::Epic:
			status_.lifeTime += 1.0f;
			break;
		case Rarity::Legendary:
			status_.lifeTime += 2.0f;
			break;
		}
		status_.upgradeCount++;
	}

	/// <summary>
	/// 貫通回数を強化（Laser専用）
	/// </summary>
	/// <param name="rarity"></param>
	void UpgradePenetrationCount(Rarity rarity) {
		switch (rarity)
		{
		case Rarity::UnCommon:
			status_.penetrationCount += 1.0f;
			break;
		case Rarity::Rare:
			status_.penetrationCount += 1.5f;
			break;
		case Rarity::Epic:
			status_.penetrationCount += 2.0f;
			break;
		case Rarity::Legendary:
			status_.penetrationCount += 3.0f;
			break;
		}
		status_.upgradeCount++;
	}

	/// <summary>
	/// 反射回数を強化（Runa専用）
	/// </summary>
	/// <param name="rarity"></param>
	void UpgradeBounceCount(Rarity rarity) {
		switch (rarity)
		{
		case Rarity::UnCommon:
			status_.bounceCount += 1.0f;
			break;
		case Rarity::Rare:
			status_.bounceCount += 1.5f;
			break;
		case Rarity::Epic:
			status_.bounceCount += 2.0f;
			break;
		case Rarity::Legendary:
			status_.bounceCount += 3.0f;
			break;
		}
		status_.upgradeCount++;
	}
	
	int GetUpgradeCount() const { return status_.upgradeCount; }

	/// <summary>
	/// この武器のキルカウントを取得
	/// </summary>
	int GetKillCount() const { return status_.killCount; }
	
	/// <summary>
	/// この武器のキルカウントをインクリメント
	/// </summary>
	void IncrementKillCount() { status_.killCount++; }

	/// <summary>
	/// 武器ステータスへの参照を取得（デバッグ用）
	/// </summary>
	const WeaponStatus& GetStatus() const { return status_; }
private:

	// 各武器の更新関数
	void FireBallUpdate();
	void LaserUpdate();
	void RunaUpdate();
	void AxeUpdate();
	void BoomerangUpdate();
	void DiceUpdate();
	void ToxicUpdate();
	void AreaUpdate();
	void GunUpdate();

	/// <summary>
	/// 共通の武器更新ロジック（テンプレート関数）
	/// </summary>
	/// <typeparam name="T">弾の型</typeparam>
	/// <param name="bulletList">弾のリスト</param>
	/// <param name="seList">再生する効果音</param>
	/// <param name="setupFunc">弾の初期化を行うラムダ関数</param>
	template<typename T>
	void UpdateWeapon(
		std::vector<std::unique_ptr<T>>& bulletList,
		SE_List seList,
		std::function<void(std::unique_ptr<T>&)> setupFunc
	) {
		// クールタイムが終了している場合
		if (coolDownTimer_.IsFinished()) {
			if (!intervalTimer_.IsActive()) {
				intervalTimer_.Start(status_.intervalTime, true);
				coolDownTimer_.Reset();
			}
		}

		if (intervalTimer_.IsFinished()) {
			auto bullet = std::make_unique<T>();
			setupFunc(bullet);
			bulletList.push_back(std::move(bullet));

			MyAudio::Play(seList);

			status_.shotNowCount++;
			if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
				status_.shotNowCount = 0;
				intervalTimer_.Reset();
				coolDownTimer_.Start(status_.cooldownTime, false);
			}
		}

		coolDownTimer_.Update();
		intervalTimer_.Update();

		for (auto& bullet : bulletList) {
			bullet->Update();
		}
	}

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