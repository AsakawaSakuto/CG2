#include "Weapon.h"
#include <algorithm>

void Weapon::Initialize(WeaponName weaponName) {

	weaponName_ = weaponName;

	switch (weaponName_)
	{
	case WeaponName::FireBall:

		status_.cooldownTime = 1.0f;
		status_.intervalTime = 0.5f;
		status_.damage = 15.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Laser:

		status_.cooldownTime = 1.05f;
		status_.intervalTime = 0.25f;
		status_.damage = 10.0f;
		status_.penetrationCount = 3.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Runa:

		status_.cooldownTime = 1.5f;
		status_.intervalTime = 0.25f;
		status_.damage = 10.0f;
		status_.bounceCount = 2.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Axe:

		status_.cooldownTime = 1.5f;
		status_.intervalTime = 0.5f;
		status_.damage = 5.0f;
		status_.lifeTime = 2.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Boomerang:

		status_.cooldownTime = 2.0f;
		status_.intervalTime = 0.25f;
		status_.damage = 5.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Dice:

		status_.cooldownTime = 10.0f;
		status_.intervalTime = 0.2f;
		status_.damage = 1.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Toxic:

		status_.cooldownTime = 1.0f;
		status_.intervalTime = 0.5f;
		status_.sizeRate = 2.0f;
		status_.damage = 5.0f;
		status_.lifeTime = 2.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Area:
		
		// Areaは初期化時に生成
		status_.damage = 3.0f;
		area_ = std::make_unique<Area>();
		area_->Initialize();
		area_->SetDamage(status_.damage);

		break;

	case WeaponName::Gun:

		status_.cooldownTime = 2.5f;
		status_.intervalTime = 0.2f;
		status_.damage = 5.0f;
		status_.shotMaxCount = 5.0f;
		status_.useRandomTarget = true; // Gunはランダムターゲット選択を使用
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;
	}
}

void Weapon::Update() {

	switch (weaponName_) {
	case WeaponName::FireBall:
		FireBallUpdate();
		break;
	case WeaponName::Laser:
		LaserUpdate();
		break;
	case WeaponName::Runa:
		RunaUpdate();
		break;
	case WeaponName::Axe:
		AxeUpdate();
		break;
	case WeaponName::Boomerang:
		BoomerangUpdate();
		break;
	case WeaponName::Dice:
		DiceUpdate();
		break;
	case WeaponName::Toxic:
		ToxicUpdate();
		break;
	case WeaponName::Area:
		AreaUpdate();
		break;
	case WeaponName::Gun:
		GunUpdate();
		break;
	}

}

void Weapon::Draw(Camera camera) {
	DrawVec(fireBall_, camera);
	DrawVec(laser_, camera);
	DrawVec(runa_, camera);
	DrawVec(axe_, camera);
	DrawVec(boomerang_, camera);
	DrawVec(dice_, camera);
	DrawVec(toxic_, camera);
	DrawVec(gun_, camera);
	
	// Areaは単一インスタンスなので個別に描画
	if (area_ && area_->IsAlive()) {
		area_->Draw(camera);
	}
}

void Weapon::PostFrameCleanup() {
	// 死亡した弾を削除し、パーティクルをプールに返却
	EraseDead(fireBall_);
	EraseDead(laser_);
	EraseDead(runa_);
	EraseDead(axe_);
	EraseDead(boomerang_);
	EraseDead(dice_);
	EraseDead(toxic_);
	EraseDead(gun_);
	// Areaは常時存在するのでクリーンアップ不要
}

void Weapon::FireBallUpdate() {
	UpdateWeapon<FireBall>(
		fireBall_,
		SE_List::FireBall,
		[this](std::unique_ptr<FireBall>& bullet) {
			bullet->Initialize();
			bullet->SetPosition(playerPosition_ + spawnOffSet_);
			bullet->SetDirectionToEnemy(directionToEnemy_);
			bullet->SetDamage(status_.damage);
			bullet->SetWeaponName(weaponName_);
		}
	);
}

void Weapon::LaserUpdate() {
	UpdateWeapon<Laser>(
		laser_,
		SE_List::Laser,
		[this](std::unique_ptr<Laser>& bullet) {
			bullet->Initialize();
			bullet->SetPosition(playerPosition_ + spawnOffSet_);
			bullet->SetDirectionToEnemy(directionToEnemy_);
			bullet->SetDamage(status_.damage);
			bullet->SetPenetrationCount(static_cast<int>(status_.penetrationCount));
			bullet->SetWeaponName(weaponName_);
		}
	);
}

void Weapon::RunaUpdate() {
	UpdateWeapon<Runa>(
		runa_,
		SE_List::Runa,
		[this](std::unique_ptr<Runa>& bullet) {
			bullet->Initialize();
			bullet->SetPosition(playerPosition_ + spawnOffSet_);
			bullet->SetDirectionToEnemy(directionToEnemy_);
			bullet->SetDamage(status_.damage);
			bullet->SetBounceCount(static_cast<int>(status_.bounceCount));
			bullet->SetWeaponName(weaponName_);
		}
	);
}

void Weapon::AxeUpdate() {
	UpdateWeapon<Axe>(
		axe_,
		SE_List::Axe,
		[this](std::unique_ptr<Axe>& bullet) {
			bullet->Initialize();
			bullet->SetPosition(playerPosition_ + spawnOffSet_);
			bullet->SetDirectionToEnemy(directionToEnemy_);
			bullet->SetDamage(status_.damage);
			bullet->SetLifeTime(status_.lifeTime);
			bullet->SetScaleRate(status_.sizeRate);
			bullet->SetWeaponName(weaponName_);
		}
	);
}

void Weapon::BoomerangUpdate() {
	UpdateWeapon<Boomerang>(
		boomerang_,
		SE_List::Boomerang,
		[this](std::unique_ptr<Boomerang>& bullet) {
			bullet->Initialize();
			bullet->SetPosition(playerPosition_ + spawnOffSet_);
			bullet->SetDirectionToEnemy(directionToEnemy_);
			bullet->SetDamage(status_.damage);
			bullet->SetScaleRate(status_.sizeRate);
			bullet->SetWeaponName(weaponName_);
		}
	);
}

void Weapon::DiceUpdate() {
	UpdateWeapon<Dice>(
		dice_,
		SE_List::Dice,
		[this](std::unique_ptr<Dice>& bullet) {
			bullet->Initialize();
			bullet->SetPosition(playerPosition_ + spawnOffSet_);
			bullet->SetDirectionToEnemy(directionToEnemy_);
			bullet->SetDamage(status_.damage);
			bullet->SetWeaponName(weaponName_);
		}
	);
}

void Weapon::ToxicUpdate() {
	UpdateWeapon<Toxic>(
		toxic_,
		SE_List::Toxic,
		[this](std::unique_ptr<Toxic>& bullet) {
			bullet->Initialize();
			bullet->SetPosition(playerPosition_);
			bullet->SetDirectionToEnemy(directionToEnemy_);
			bullet->SetDamage(status_.damage);
			bullet->SetScaleMultipler(status_.sizeRate);
			bullet->SetLifeTime(status_.lifeTime);
			bullet->SetWeaponName(weaponName_);
		}
	);
}

void Weapon::AreaUpdate() {
	// Areaは常にプレイヤーの位置に追従
	if (area_) {
		area_->SetPosition(playerPosition_);
		area_->SetDamage(status_.damage);
		area_->SetScaleMultipler(status_.sizeRate);
		area_->SetWeaponName(weaponName_); // 武器の種類を設定
		area_->Update();
	}
}

void Weapon::GunUpdate() {
	UpdateWeapon<Gun>(
		gun_,
		SE_List::Gun,
		[this](std::unique_ptr<Gun>& bullet) {
			bullet->Initialize();
			bullet->SetPosition(playerPosition_ + spawnOffSet_);
			// ランダムターゲット選択が有効な場合は Random DirectionToEnemy_を使用
			if (status_.useRandomTarget) {
				bullet->SetDirectionToEnemy(randomDirectionToEnemy_);
			} else {
				bullet->SetDirectionToEnemy(directionToEnemy_);
			}
			bullet->SetDamage(status_.damage);
			bullet->SetWeaponName(weaponName_);
		}
	);
}

void Weapon::SetWeaponName(WeaponName weapon) {
	if (weaponName_ == WeaponName::None) {
		weaponName_ = weapon;

		switch (weaponName_)
		{
		case WeaponName::FireBall:

			status_.cooldownTime = 1.0f;
			status_.intervalTime = 0.5f;
			status_.damage = 10.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;

		case WeaponName::Laser:

			status_.cooldownTime = 2.0f;
			status_.intervalTime = 0.25f;
			status_.damage = 7.0f;
			status_.penetrationCount = 3.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;

		case WeaponName::Runa:

			status_.cooldownTime = 2.0f;
			status_.intervalTime = 0.25f;
			status_.damage = 8.0f;
			status_.bounceCount = 2.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;

		case WeaponName::Axe:

			status_.cooldownTime = 1.5f;
			status_.intervalTime = 0.5f;
			status_.damage = 5.0f;
			status_.lifeTime = 2.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;

		case WeaponName::Boomerang:

			status_.cooldownTime = 2.0f;
			status_.intervalTime = 0.25f;
			status_.damage = 3.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;

		case WeaponName::Dice:

			status_.cooldownTime = 1.0f;
			status_.intervalTime = 0.2f;
			status_.damage = 1.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;

		case WeaponName::Toxic:

			status_.cooldownTime = 1.0f;
			status_.intervalTime = 0.5f;
			status_.sizeRate = 2.0f;
			status_.damage = 2.0f;
			status_.lifeTime = 2.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;

		case WeaponName::Area:

			// Areaは初期化時に生成
			status_.damage = 3.0f;
			area_ = std::make_unique<Area>();
			area_->Initialize();
			area_->SetDamage(status_.damage);

			break;

		case WeaponName::Gun:

			status_.cooldownTime = 2.5f;
			status_.intervalTime = 0.2f;
			status_.damage = 3.0f;
			status_.shotMaxCount = 5.0f;
			status_.useRandomTarget = true; // Gunはランダムターゲット選択を使用
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;
		}
	}
}