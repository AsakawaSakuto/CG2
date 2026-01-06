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

		status_.cooldownTime = 1.05;
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
		bullet->SetWeaponName(weaponName_); // 武器の種類を設定
		fireBall_.push_back(std::move(bullet));

		MyAudio::PlaySE(SE_List::FireBall);

		status_.shotNowCount++;
		if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
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
		bullet->SetPenetrationCount(static_cast<int>(status_.penetrationCount));
		bullet->SetWeaponName(weaponName_); // 武器の種類を設定
		laser_.push_back(std::move(bullet));

		MyAudio::PlaySE(SE_List::Laser);

		status_.shotNowCount++;
		if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
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
		bullet->SetBounceCount(static_cast<int>(status_.bounceCount));
		bullet->SetWeaponName(weaponName_); // 武器の種類を設定
		runa_.push_back(std::move(bullet));

		MyAudio::PlaySE(SE_List::Runa);

		status_.shotNowCount++;
		if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
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
		bullet->SetScaleRate(status_.sizeRate);
		bullet->SetWeaponName(weaponName_); // 武器の種類を設定
		axe_.push_back(std::move(bullet));

		MyAudio::PlaySE(SE_List::Axe);

		status_.shotNowCount++;
		if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
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
		bullet->SetScaleRate(status_.sizeRate);
		bullet->SetWeaponName(weaponName_); // 武器の種類を設定
		boomerang_.push_back(std::move(bullet));

		MyAudio::PlaySE(SE_List::Boomerang);

		status_.shotNowCount++;
		if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
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
		bullet->SetWeaponName(weaponName_); // 武器の種類を設定
		dice_.push_back(std::move(bullet));

		MyAudio::PlaySE(SE_List::Dice);

		status_.shotNowCount++;
		if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
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
		bullet->SetScaleMultipler(status_.sizeRate);
		bullet->SetLifeTime(status_.lifeTime);
		bullet->SetWeaponName(weaponName_); // 武器の種類を設定
		toxic_.push_back(std::move(bullet));

		MyAudio::PlaySE(SE_List::Toxic);

		status_.shotNowCount++;
		if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
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
	// クールタイムが終了している場合
	if (coolDownTimer_.IsFinished()) {
		if (!intervalTimer_.IsActive()) {
			intervalTimer_.Start(status_.intervalTime, true);
			coolDownTimer_.Reset();
		}
	}
	if (intervalTimer_.IsFinished()) {
		auto bullet = std::make_unique<Gun>();
		bullet->Initialize();
		bullet->SetPosition(playerPosition_ + spawnOffSet_);
		// ランダムターゲット選択が有効な場合は Random DirectionToEnemy_を使用
		if (status_.useRandomTarget) {
			bullet->SetDirectionToEnemy(randomDirectionToEnemy_);
		} else {
			bullet->SetDirectionToEnemy(directionToEnemy_);
		}
		bullet->SetDamage(status_.damage);
		bullet->SetWeaponName(weaponName_); // 武器の種類を設定
		gun_.push_back(std::move(bullet));
		status_.shotNowCount++;

		MyAudio::PlaySE(SE_List::Gun);

		if (status_.shotNowCount >= static_cast<int>(status_.shotMaxCount)) {
			status_.shotNowCount = 0;
			intervalTimer_.Reset();
			coolDownTimer_.Start(status_.cooldownTime, false);
		}
	}
	coolDownTimer_.Update();
	intervalTimer_.Update();
	for (auto& bullet : gun_) {
		bullet->Update();
	}
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