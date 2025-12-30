#pragma once
#include "GameObject/BaseGameObject.h"
#include "EngineSystem.h"
#include "playerStatus.h"
#include "AnimationController/AnimationController.h"
#include "GameObject/Player/WeaponManager/WeaponManager.h"

// 前方宣言
class EnemyManager;
class Map3D;
class TreeManager;

class Player : public BaseGameObject {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;
  
	Sphere& GetExpItemStateChangeCollision() { return expItemStateChangeCollision_; }

	// EnemyManagerへの参照を設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

	// Map3Dへの参照を設定
	void SetMap(Map3D* map) { map_ = map; }
	
	// TreeManagerへの参照を設定
	void SetTreeManager(TreeManager* treeManager) { treeManager_ = treeManager; }

	// WeaponManagerへのアクセス
	WeaponManager* GetWeaponManager() { return weaponManager_.get(); }

	void PostFrameCleanup();
	
	/// <summary>
	/// 現在のHPを取得
	/// </summary>
	int GetCurrentHP() const { return status_.currentHP; }

	/// <summary>
	/// 最大HPを取得
	/// </summary>
	int GetMaxHP() const { return status_.maxHP; }

	/// <summary>
	/// 現在の経験値を取得
	/// </summary>
	int GetCurrentExp() const { return status_.currentExp; }

	/// <summary>
	/// 次のレベルまでに必要な経験値を取得
	/// </summary>
	int GetExpToNextLevel() const { return status_.expToNextLevel; }

	/// <summary>
	/// /
	/// </summary>
	/// <returns></returns>
	int GetNowMoney() const { return status_.nowMoney; }

	/// <summary>
	/// 現在のレベルを取得
	/// </summary>
	int GetLevel() const { return status_.level; }

	/// <summary>
	/// マップ衝突用AABBを取得
	/// </summary>
	const AABB& GetMapCollisionAABB() const { return mapCollosion_; }

	/// <summary>
	/// HPを設定（ダメージ・回復用）
	/// </summary>
	void SetCurrentHP(int hp);

	/// <summary>
	/// 経験値を追加
	/// </summary>
	void AddExp(int exp);

	void AddMoney(int money);

	/// <summary>
	/// お金を減らす（戻り値: 減らせたかどうか）
	/// </summary>
	bool SubtractMoney(int money);

	/// <summary>
	/// 敵を倒したカウントをインクリメント
	/// </summary>
	void IncrementKillEnemyCount() { status_.killEnemyCount++; }

	int GetKillEnemyCount() const { return status_.killEnemyCount; }

	/// <summary>
	/// Playerの位置を設定する
	/// </summary>
	void SetPosition(const Vector3& position) { transform_.translate = position; }
	
private:

	// 移動処理
	void Move();

	// ジャンプ処理
	void Jump();

	// しゃがみ中のスロープ滑り処理
	void SlideOnSlope();

	// カメラの向きに基づいた移動方向を計算
	Vector3 CalculateCameraMoveDirection(float stickX, float stickY);

	// プレイヤーから最も近い敵へのベクトルを取得（正規化されていない）
	Vector3 GetDirectionToEnemy() const;

	// プレイヤーから最も近い敵までの距離を取得
	float GetDistanceToNearestEnemy() const;

	/// <summary>
	/// マップとの衝突を解決する（めり込み防止）
	/// </summary>
	void ResolveMapCollision();

	/// <summary>
	/// 地面に接地しているかをマップのAABBで判定
	/// </summary>
	/// <returns>地面に接している場合true</returns>
	bool IsGroundedOnMap();

private:
	unique_ptr<AnimationController> model_;
	PlayerMotion currentMotion_ = PlayerMotion::Idle;

	Transform expGetRangeTransform_;

	unique_ptr<Particles> moveParticle_ = make_unique<Particles>();
	unique_ptr<Particles> landingParticle_ = make_unique<Particles>();

	unique_ptr<WeaponManager> weaponManager_ = make_unique<WeaponManager>();

	Camera camera_; // 現在のカメラへの参照
	
	PlayerStatus status_; // プレイヤーステータス

	Vector3 directionToEnemy_ = { 0.0f, 0.0f, 0.0f };

	float collisionRadius_ = 0.5f;

	// ジャンプ関連のメンバ変数
	float groundLevel_ = 2.5f; // 地面のY座標（廃止予定）
	bool isGrounded_ = true;   // 地面にいるかどうか
	bool wasGrounded_ = true;  // 前フレームで地面にいたかどうか

	// スロープ滑り関連
	float slideSpeed_ = 20.0f;  // スロープでの滑り速度

	// EnemyManagerへの参照（生ポインタ、所有権なし)
	EnemyManager* enemyManager_ = nullptr;

	// Map3Dへの参照（生ポインタ、所有権なし）
	Map3D* map_ = nullptr;
	
	// TreeManagerへの参照（生ポインタ、所有権なし）
	TreeManager* treeManager_ = nullptr;

	Sphere expItemStateChangeCollision_;
	float expItemStateChangeRadius_ = 3.5f;

	AABB mapCollosion_;

	// フォールバック機構：前フレームの位置を記録
	Vector3 previousFramePosition_ = { 0.0f, 0.0f, 0.0f };
	bool hasPreviousPosition_ = false; // 前フレーム位置が有効かどうか
};