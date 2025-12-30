#include "Weapon.h"
#include <algorithm>

void Weapon::Initialize(WeaponName weaponName) {

	weaponName_ = weaponName;

	switch (weaponName_)
	{
	case WeaponName::FireBall:

		status_.cooldownTime = 2.0f;
		status_.intervalTime = 0.2f;
		status_.shotMaxCount = 5;
		status_.shotNowCount = 0;
		status_.size = 1.0f;
		status_.damage = 10.0f;
		status_.criticalRand = 10;
		status_.moveSpeed = 15.0f;
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
		status_.size = 1.0f;
		status_.damage = 10.0f;
		status_.criticalRand = 10;
		status_.moveSpeed = 20.0f;
		status_.bounceCount = 0;
		status_.penetrationCount = 99;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Runa:

		status_.cooldownTime = 3.0f;
		status_.intervalTime = 1.0f;
		status_.shotMaxCount = 2;
		status_.shotNowCount = 0;
		status_.size = 1.0f;
		status_.damage = 10.0f;
		status_.criticalRand = 10;
		status_.moveSpeed = 7.5f;
		status_.bounceCount = 3;
		status_.penetrationCount = 0;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
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
		break;
	case WeaponName::Runa:
		break;
	}

}

void Weapon::Draw(Camera camera) {
	for (auto& fireball : fireBall_) {
		fireball->Draw(camera);
	}
}

void Weapon::PostFrameCleanup() {
	// 死亡した弾を削除し、パーティクルをプールに返却
	auto it = fireBall_.begin();
	while (it != fireBall_.end()) {
		if (!(*it)->IsAlive()) {
			it = fireBall_.erase(it);
		} else {
			++it;
		}
	}
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

		auto fireBall = std::make_unique<FireBall>();
		fireBall->Initialize();
		fireBall->SetPosition(playerPosition_ + spawnOffSet_);
		fireBall->SetDirectionToEnemy(directionToEnemy_);
		fireBall_.push_back(std::move(fireBall));

		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}

	coolDownTimer_.Update();
	intervalTimer_.Update();

	for (auto& fireBall : fireBall_) {
		fireBall->Update();
	}
}