#pragma once
#include "GameObject/BaseGameObject.h"
#include "EngineSystem.h"
#include "playerStatus.h"
#include "GameObject/Player/WeaponManager/WeaponManager.h"

// 前方宣言
class EnemyManager;

class Player : public BaseGameObject {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;
  
	Sphere& GetExpItemStateChangeCollision() { return expItemStateChangeCollision_; }

	// EnemyManagerへの参照を設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

	// WeaponManagerへのアクセス
	WeaponManager* GetWeaponManager() { return weaponManager_.get(); }

	void PostFrameCleanup();

	// === ステータス取得メソッド ===
	
	/// <summary>
	/// 現在のHPを取得
	/// </summary>
	int GetCurrentHP() const { return status_.currentHP_; }

	/// <summary>
	/// 最大HPを取得
	/// </summary>
	int GetMaxHP() const { return status_.maxHP_; }

	/// <summary>
	/// 現在の経験値を取得
	/// </summary>
	int GetCurrentExp() const { return status_.currentExp_; }

	/// <summary>
	/// 次のレベルまでに必要な経験値を取得
	/// </summary>
	int GetExpToNextLevel() const { return status_.expToNextLevel_; }

	/// <summary>
	/// 現在のレベルを取得
	/// </summary>
	int GetLevel() const { return status_.level_; }

	/// <summary>
	/// HPを設定（ダメージ・回復用）
	/// </summary>
	void SetCurrentHP(int hp);

	/// <summary>
	/// 経験値を追加
	/// </summary>
	void AddExp(int exp);

private:

	// 移動処理
	void Move();

	// ジャンプ処理
	void Jump();

	// カメラの向きに基づいた移動方向を計算
	Vector3 CalculateCameraMoveDirection(float stickX, float stickY);

	// プレイヤーから最も近い敵へのベクトルを取得（正規化されていない）
	Vector3 GetDirectionToEnemy() const;

	// プレイヤーから最も近い敵までの距離を取得
	float GetDistanceToNearestEnemy() const;

private:
	unique_ptr<Model> model_ = make_unique<Model>();
	unique_ptr<Model> expItemGetRange_ = make_unique<Model>();

	Transform expGetRangeTransform_;

	unique_ptr<Particles> moveParticle_ = make_unique<Particles>();
	unique_ptr<Particles> landingParticle_ = make_unique<Particles>();

	unique_ptr<WeaponManager> weaponManager_ = make_unique<WeaponManager>();

	Camera camera_; // 現在のカメラへの参照
	
	PlayerStatus status_; // プレイヤーステータス

	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };

	float collisionRadius_ = 0.5f;

	// ジャンプ関連のメンバ変数
	float groundLevel_ = 0.0f; // 地面のY座標
	bool isGrounded_ = true;   // 地面にいるかどうか
	bool wasGrounded_ = true;  // 前フレームで地面にいたかどうか

	// EnemyManagerへの参照（生ポインタ、所有権なし)
	EnemyManager* enemyManager_ = nullptr;

	Sphere expItemStateChangeCollision_;
	float expItemStateChangeRadius_ = 3.5f;
};