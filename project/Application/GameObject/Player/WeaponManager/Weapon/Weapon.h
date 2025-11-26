#pragma once
#include "Application/AppContext.h"
#include "Application/GameObject/Player/WeaponManager/Weapon/Bullet/bullet.h"
#include "Application/GameObject/Player/WeaponManager/WeaponStatus.h"
#include "Application/GameObject/Player/WeaponManager/WeaponName.h"
#include "Camera.h"
#include <queue>

class Weapon {
public:
	void Initialize(AppContext* ctx, WeaponName weaponName);

	void Update();

	void Draw(Camera camera);

	void SetPlayerPosition(const Vector3& position);
	void SetDirectionToEnemy(const Vector3& direction);

	// 弾のリストへのアクセス（const参照）
	const std::vector<std::unique_ptr<Bullet>>& GetBullets() const { return bullets_; }

	void PostFrameCleanup();
	
private:
	AppContext* ctx_;
	std::vector<std::unique_ptr<Bullet>> bullets_;
	
	// パーティクルオブジェクトプール
	std::queue<std::shared_ptr<Particles>> particlePool_;
	const size_t kMaxPoolSize_ = 20; // プールの最大サイズ

	WeaponStatus status_;
	GameTimer coolDownTimer_; // クールタイムタイマー
	GameTimer intervalTimer_; // 攻撃間隔タイマー

	Vector3 playerPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };
	
	// プールからパーティクルを取得
	std::shared_ptr<Particles> AcquireParticleFromPool();
	// プールにパーティクルを返却
	void ReturnParticleToPool(std::shared_ptr<Particles> particle);
};