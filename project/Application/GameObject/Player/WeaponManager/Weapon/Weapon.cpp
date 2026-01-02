#include "Weapon.h"
#include <algorithm>

void Weapon::Initialize(WeaponName weaponName) {

	weaponName_ = weaponName;

	switch (weaponName_)
	{
	case WeaponName::FireBall:

		status_.cooldownTime = 2.0f;
		status_.intervalTime = 0.2f;
		status_.shotMaxCount = 1;
		status_.shotNowCount = 0;
		status_.damage = 10.0f;
		status_.criticalRand = 10;
		status_.bounceCount = 0;
		status_.penetrationCount = 0;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Laser:

		status_.cooldownTime = 5.0f;
		status_.intervalTime = 0.5f;
		status_.shotMaxCount = 1;
		status_.shotNowCount = 0;
		status_.damage = 5.0f;
		status_.criticalRand = 10;
		status_.bounceCount = 0;
		status_.penetrationCount = 2;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Runa:

		status_.cooldownTime = 3.0f;
		status_.intervalTime = 1.0f;
		status_.shotMaxCount = 2;
		status_.shotNowCount = 0;
		status_.damage = 5.0f;
		status_.criticalRand = 10;
		status_.bounceCount = 2;
		status_.penetrationCount = 0;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Axe:

		status_.cooldownTime = 3.0f;
		status_.intervalTime = 1.0f;
		status_.shotMaxCount = 1;
		status_.shotNowCount = 0;
		status_.damage = 5.0f;
		status_.criticalRand = 10;
		status_.bounceCount = 0;
		status_.penetrationCount = 0;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		status_.lifeTime = 5.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Boomerang:

		status_.cooldownTime = 2.0f;
		status_.intervalTime = 1.0f;
		status_.shotMaxCount = 1;
		status_.shotNowCount = 0;
		status_.damage = 3.0f;
		status_.criticalRand = 10;
		status_.bounceCount = 0;
		status_.penetrationCount = 0;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		status_.lifeTime = 5.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Dice:

		status_.cooldownTime = 4.0f;
		status_.intervalTime = 1.0f;
		status_.shotMaxCount = 1;
		status_.shotNowCount = 0;
		status_.damage = 1.0f;
		status_.criticalRand = 10;
		status_.bounceCount = 0;
		status_.penetrationCount = 0;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		status_.lifeTime = 10.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Toxic:

		status_.cooldownTime = 1.0f;
		status_.intervalTime = 1.0f;
		status_.shotMaxCount = 1;
		status_.shotNowCount = 0;
		status_.damage = 1.0f;
		status_.criticalRand = 10;
		status_.bounceCount = 0;
		status_.penetrationCount = 0;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		status_.lifeTime = 5.0f;
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
}

void Weapon::FireBallUpdate() {
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}

	if (intervalTimer_.IsFinished()) {

		auto bullet = std::make_unique<FireBall>();
		bullet->Initialize();
		bullet->SetPosition(playerPosition_ + spawnOffSet_);
		bullet->SetDirectionToEnemy(directionToEnemy_);
		bullet->SetDamage(status_.damage);
		fireBall_.push_back(std::move(bullet));

		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}

	coolDownTimer_.Update();
	intervalTimer_.Update();

	for (auto& bullet : fireBall_) {
		bullet->Update();
	}
}

void Weapon::LaserUpdate() {
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}

	if (intervalTimer_.IsFinished()) {

		auto bullet = std::make_unique<Laser>();
		bullet->Initialize();
		bullet->SetPosition(playerPosition_ + spawnOffSet_);
		bullet->SetDirectionToEnemy(directionToEnemy_);
		bullet->SetDamage(status_.damage);
		bullet->SetPenetrationCount(status_.penetrationCount);
		laser_.push_back(std::move(bullet));

		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}

	coolDownTimer_.Update();
	intervalTimer_.Update();

	for (auto& bullet : laser_) {
		bullet->Update();
	}
}

void Weapon::RunaUpdate() {
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}
	if (intervalTimer_.IsFinished()) {
		auto bullet = std::make_unique<Runa>();
		bullet->Initialize();
		bullet->SetPosition(playerPosition_ + spawnOffSet_);
		bullet->SetDirectionToEnemy(directionToEnemy_);
		bullet->SetDamage(status_.damage);
		bullet->SetBounceCount(status_.bounceCount);
		runa_.push_back(std::move(bullet));
		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}
	coolDownTimer_.Update();
	intervalTimer_.Update();
	for (auto& bullet : runa_) {
		bullet->Update();
	}
}

void Weapon::AxeUpdate() {
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}
	if (intervalTimer_.IsFinished()) {
		auto bullet = std::make_unique<Axe>();
		bullet->Initialize();
		bullet->SetPosition(playerPosition_ + spawnOffSet_);
		bullet->SetDirectionToEnemy(directionToEnemy_);
		bullet->SetDamage(status_.damage);
		bullet->SetLifeTime(status_.lifeTime);
		axe_.push_back(std::move(bullet));
		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}
	coolDownTimer_.Update();
	intervalTimer_.Update();
	for (auto& bullet : axe_) {
		bullet->Update();
	}
}

void Weapon::BoomerangUpdate() {
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}
	if (intervalTimer_.IsFinished()) {
		auto bullet = std::make_unique<Boomerang>();
		bullet->Initialize();
		bullet->SetPosition(playerPosition_ + spawnOffSet_);
		bullet->SetDirectionToEnemy(directionToEnemy_);
		bullet->SetDamage(status_.damage);
		boomerang_.push_back(std::move(bullet));
		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}
	coolDownTimer_.Update();
	intervalTimer_.Update();
	for (auto& bullet : boomerang_) {
		bullet->Update();
	}
}

void Weapon::DiceUpdate() {
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}
	if (intervalTimer_.IsFinished()) {
		auto bullet = std::make_unique<Dice>();
		bullet->Initialize();
		bullet->SetPosition(playerPosition_ + spawnOffSet_);
		bullet->SetDirectionToEnemy(directionToEnemy_);
		bullet->SetDamage(status_.damage);
		dice_.push_back(std::move(bullet));
		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}
	coolDownTimer_.Update();
	intervalTimer_.Update();
	for (auto& bullet : dice_) {
		bullet->Update();
	}
}

void Weapon::ToxicUpdate() {
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}
	if (intervalTimer_.IsFinished()) {
		auto bullet = std::make_unique<Toxic>();
		bullet->Initialize();
		bullet->SetPosition(playerPosition_);
		bullet->SetDirectionToEnemy(directionToEnemy_);
		bullet->SetDamage(status_.damage);
		toxic_.push_back(std::move(bullet));
		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}
	coolDownTimer_.Update();
	intervalTimer_.Update();
	for (auto& bullet : toxic_) {
		bullet->Update();
	}
}

void Weapon::SetWeaponName(WeaponName weapon) {
	if (weaponName_ == WeaponName::None) {
		weaponName_ = weapon;

		switch (weaponName_) {
		case WeaponName::FireBall:

			status_.cooldownTime = 2.0f;
			status_.intervalTime = 0.2f;
			status_.shotMaxCount = 1;
			status_.shotNowCount = 0;
			status_.damage = 10.0f;
			status_.criticalRand = 10;
			status_.bounceCount = 0;
			status_.penetrationCount = 0;
			status_.nockBackPower = 0.0f;
			status_.durationTime = 0.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;
		case WeaponName::Laser:

			status_.cooldownTime = 5.0f;
			status_.intervalTime = 0.5f;
			status_.shotMaxCount = 1;
			status_.shotNowCount = 0;
			status_.damage = 5.0f;
			status_.criticalRand = 10;
			status_.bounceCount = 0;
			status_.penetrationCount = 2;
			status_.nockBackPower = 0.0f;
			status_.durationTime = 0.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;
		case WeaponName::Runa:

			status_.cooldownTime = 3.0f;
			status_.intervalTime = 1.0f;
			status_.shotMaxCount = 2;
			status_.shotNowCount = 0;
			status_.damage = 5.0f;
			status_.criticalRand = 10;
			status_.bounceCount = 2;
			status_.penetrationCount = 0;
			status_.nockBackPower = 0.0f;
			status_.durationTime = 0.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;
		case WeaponName::Axe:

			status_.cooldownTime = 3.0f;
			status_.intervalTime = 1.0f;
			status_.shotMaxCount = 1;
			status_.shotNowCount = 0;
			status_.damage = 5.0f;
			status_.criticalRand = 10;
			status_.bounceCount = 0;
			status_.penetrationCount = 0;
			status_.nockBackPower = 0.0f;
			status_.durationTime = 0.0f;
			status_.lifeTime = 5.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;
		case WeaponName::Dice:

			status_.cooldownTime = 4.0f;
			status_.intervalTime = 1.0f;
			status_.shotMaxCount = 1;
			status_.shotNowCount = 0;
			status_.damage = 1.0f;
			status_.criticalRand = 10;
			status_.bounceCount = 0;
			status_.penetrationCount = 0;
			status_.nockBackPower = 0.0f;
			status_.durationTime = 0.0f;
			status_.lifeTime = 10.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;
		case WeaponName::Toxic:

			status_.cooldownTime = 1.0f;
			status_.intervalTime = 1.0f;
			status_.shotMaxCount = 1;
			status_.shotNowCount = 0;
			status_.damage = 1.0f;
			status_.criticalRand = 10;
			status_.bounceCount = 0;
			status_.penetrationCount = 0;
			status_.nockBackPower = 0.0f;
			status_.durationTime = 0.0f;
			status_.lifeTime = 5.0f;
			coolDownTimer_.Start(status_.cooldownTime, false);

			break;
		default:
			break;
		}
	}
}