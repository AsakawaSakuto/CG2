#include "Weapon.h"
#include <algorithm>

void Weapon::Initialize(AppContext* ctx) {
	ctx_ = ctx;

	status_.cooldownTime = 2.0f;
	status_.intervalTime = 0.2f;
	status_.shotMaxCount = 5;
	status_.shotNowCount = 0;
	status_.size = 1.0f;
	status_.damage = 10.0f;
	status_.criticalRand = 10;
	status_.moveSpeed = 10.0f;
	status_.bounceCount = 1;
	status_.nockBackPower = 0.0f;
	status_.durationTime = 0.0f;
	coolDownTimer_.Start(status_.cooldownTime, false);
}

void Weapon::Update() {

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

	// 死亡した弾を削除
	bullets_.erase(
		std::remove_if(bullets_.begin(), bullets_.end(),
			[](const std::unique_ptr<Bullet>& bullet) {
				return !bullet->IsAlive();
			}),
		bullets_.end()
	);
}

void Weapon::Draw(Camera camera) {
	for (auto& bullet : bullets_) {
		bullet->Draw(camera);
	}
}

void Weapon::SetPlayerPosition(const Vector3& position) {
	playerPosition_ = position;
}

void Weapon::SetDirectionToEnemy(const Vector3& direction) {
	directionToEnemy_ = direction;
}