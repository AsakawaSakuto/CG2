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
	status_.moveSpeed = 5.0f;
	status_.bounceCount = 1;
	status_.nockBackPower = 0.0f;
	status_.durationTime = 0.0f;
	coolDownTimer_.Start(status_.cooldownTime, false);
	
	// パーティクルプールの初期化（重い初期化は最初の一度だけ）
	for (size_t i = 0; i < kMaxPoolSize_; ++i) {
		auto particle = std::make_shared<Particles>();
		particle->Initialize(&ctx_->dxCommon);
		particle->LoadJson("fireBall");
		particle->Stop(); // 初期状態は停止
		particlePool_.push(particle);
	}
}

std::shared_ptr<Particles> Weapon::AcquireParticleFromPool() {
	std::shared_ptr<Particles> particle;
	
	if (!particlePool_.empty()) {
		particle = particlePool_.front();
		particlePool_.pop();
	} else {
		// プールが空の場合は新規作成（通常は発生しないが、念のため）
		particle = std::make_shared<Particles>();
		particle->Initialize(&ctx_->dxCommon);
		particle->LoadJson("fireBall");
	}
	
	// パーティクルの状態を完全にリセット（軽量版）
	particle->Reset();
	
	return particle;
}

void Weapon::ReturnParticleToPool(std::shared_ptr<Particles> particle) {
	if (!particle) {
		return; // nullチェック
	}
	
	// パーティクルを完全にリセット
	particle->Reset();
	
	// プールサイズの上限チェック
	if (particlePool_.size() < kMaxPoolSize_) {
		particlePool_.push(particle);
	}
	// 上限を超えた場合は破棄される（shared_ptrなので自動削除）
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
		// プールからパーティクルを取得
		bullet->SetSharedParticle(AcquireParticleFromPool());
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

void Weapon::PostFrameCleanup() {
	// 死亡した弾を削除し、パーティクルをプールに返却
	auto it = bullets_.begin();
	while (it != bullets_.end()) {
		if (!(*it)->IsAlive()) {
			// パーティクルをプールに返却
			ReturnParticleToPool((*it)->ReleaseParticle());
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}
}