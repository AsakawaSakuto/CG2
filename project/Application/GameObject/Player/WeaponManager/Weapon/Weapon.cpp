#include "Weapon.h"
#include <algorithm>

void Weapon::Initialize(AppContext* ctx, WeaponName weaponName) {

	ctx_ = ctx;
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
		status_.weaponType = WeaponType::BulletType;
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
		status_.weaponType = WeaponType::BulletType;
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
		status_.weaponType = WeaponType::BulletType;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::BubbleArea:

		status_.cooldownTime = 10.0f;
		status_.intervalTime = 1.0f;
		status_.shotMaxCount = 1;
		status_.shotNowCount = 0;
		status_.size = 1.0f;
		status_.damage = 10.0f;
		status_.criticalRand = 10;
		status_.moveSpeed = 0.0f;
		status_.bounceCount = 0;
		status_.penetrationCount = 0;
		status_.nockBackPower = 0.0f;
		status_.durationTime = 0.0f;
		status_.weaponType = WeaponType::AreaType;
		coolDownTimer_.Start(status_.cooldownTime, false);

		break;

	case WeaponName::Sword:
		break;
	case WeaponName::Thunder:
		break;
	default:
		break;
	}
}

void Weapon::Update() {

	switch (status_.weaponType) {
	case WeaponType::BulletType:

		BulletTypeUpdate();

		break;
	case WeaponType::AreaType:

		AreaTypeUpdate();

		break;
	case WeaponType::DirectType:

		DirectTypeUpdate();

		break;
	}

}

void Weapon::Draw(Camera camera) {
	for (auto& bullet : bullets_) {
		bullet->Draw(camera);
	}

	for (auto& area : areas_) {
		area->Draw(camera);
	}
}

void Weapon::SetPlayerPosition(const Vector3& position) {
	playerPosition_ = position;
}

void Weapon::SetDirectionToEnemy(const Vector3& direction) {
	directionToEnemy_ = direction;
}

void Weapon::PostFrameCleanup() {
	// 死亡した弾を削除し、パーティクルをプールに返却
	auto it = bullets_.begin();
	while (it != bullets_.end()) {
		if (!(*it)->IsAlive()) {
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}
}

void Weapon::BulletTypeUpdate() {
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}

	if (intervalTimer_.IsFinished()) {

		auto bullet = std::make_unique<Bullet>();
		bullet->Initialize(ctx_);
		bullet->SetPosition(playerPosition_);
		bullet->SetDirectionToEnemy(directionToEnemy_);
		bullet->SetSpeed(status_.moveSpeed);
		switch (weaponName_)
		{
		case WeaponName::FireBall:
			bullet->LoadJson("fireBall", "fireBall2");
			break;
		case WeaponName::Laser:
			bullet->LoadJson("laser", "laser2");
			bullet->SetPenetrationCount(status_.penetrationCount);
			break;
		case WeaponName::Runa:
			bullet->LoadJson("runa", "runa2");
			bullet->SetBounceCount(status_.bounceCount);
			break;
		}
		bullets_.push_back(std::move(bullet));

		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}

	coolDownTimer_.Update();
	intervalTimer_.Update();

	for (auto& bullet : bullets_) {
		bullet->Update();
	}
}

void Weapon::AreaTypeUpdate() {

	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}

	if (intervalTimer_.IsFinished()) {

		auto area = std::make_unique<Area>();
		area->Initialize(ctx_);
		switch (weaponName_)
		{
		case WeaponName::BubbleArea:
			area->LoadJson("bubbleArea");
			break;
		}
		areas_.push_back(std::move(area));

		status_.shotNowCount++;
		if (status_.shotNowCount >= status_.shotMaxCount) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}

	coolDownTimer_.Update();
	intervalTimer_.Update();

	for (auto& area : areas_) {
		area->SetPosition(playerPosition_);
		area->Update();
	}

}

void Weapon::DirectTypeUpdate() {
}