#pragma once
#include "GameObject/BaseGameObject.h"
#include "EngineSystem.h"
#include "playerStatus.h"
#include "AnimationController/AnimationController.h"
#include "GameObject/Player/WeaponManager/WeaponManager.h"
#include "UpgradeManager/UpgradeManager.h"

// 前方宣言
class EnemyManager;
class Map3D;
class TreeManager;

class Player : public BaseGameObject {
public:
    void Initialize() override;
    void Initialize(PlayerName playerName, WeaponName weaponName);
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

	/// <summary>
	/// WeaponManagerへのアクセス
	/// </summary>
	WeaponManager* GetWeaponManager() { return weaponManager_.get(); }
	
	/// <summary>
	/// UpgradeManagerへのアクセス
	/// </summary>
	UpgradeManager* GetUpgradeManager() { return upgradeManager_.get(); }

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
	/// ダメージを受ける
	/// </summary>
	/// <param name="damage">受けるダメージ量</param>
	void TakeDamage(int damage);

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
	
	/// <summary>
	/// 新しい武器を装備する
	/// </summary>
	/// <param name="weaponName">装備する武器の種類</param>
	/// <returns>装備に成功した場合true</returns>
	bool EquipWeapon(WeaponName weaponName) {
		return weaponManager_->EquipWeapon(weaponName);
	}

	/// <summary>
	/// 指定スロットの武器を変更する
	/// </summary>
	/// <param name="slotIndex">変更するスロット番号（0-3）</param>
	/// <param name="weaponName">新しい武器の種類</param>
	/// <returns>変更に成功した場合true</returns>
	bool ChangeWeapon(int slotIndex, WeaponName weaponName) {
		return weaponManager_->ChangeWeapon(slotIndex, weaponName);
	}

	/// <summary>
	/// 指定の武器を持っているか確認
	/// </summary>
	bool HasWeapon(WeaponName weaponName) const {
		return weaponManager_->HasWeapon(weaponName);
	}

	/// <summary>
	/// 武器が装備可能か確認
	/// </summary>
	bool CanEquipWeapon() const {
		return weaponManager_->CanEquipWeapon();
	}

	bool IsUpgradeSelect() const {
		return upgradeManager_->IsUpgradeSelect();
	}
	
	float GetDamageRate() const { return status_.damageRate; }
	
	/// <summary>
	/// 指定した武器のキルカウントを取得
	/// </summary>
	/// <param name="weaponName">キルカウントを取得する武器の種類</param>
	/// <returns>指定した武器のキルカウント（武器を持っていない場合は0）</returns>
	int GetWeaponKillCount(WeaponName weaponName) const {
		return weaponManager_->GetWeaponKillCount(weaponName);
	}
	
	/// <summary>
	/// 全ての武器のキルカウントの合計を取得
	/// </summary>
	int GetTotalWeaponKillCount() const {
		return weaponManager_->GetTotalWeaponKillCount();
	}

	/// <summary>
	/// Dieが完全に終了
	/// </summary>
	/// <returns></returns>
	bool IsDie() const { return isDie_; }

	void SetStartPos(const Vector3& pos) {
		startPos_ = pos;
		transform_.translate = pos;
	}
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

	// プレイヤーから最も近い敵へのベクトルを取得（ランダム化なし）
	Vector3 GetDirectionToClosestEnemy() const;

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

	/// <summary>
	/// プレイヤーの真下の地面の高さを取得
	/// </summary>
	/// <returns>地面のY座標</returns>
	float GetGroundHeight() const;

private:
	bool isDie_ = false;

	float comeBackTimer_ = 0.0f;

	GameTimer healingTimer_;

	Vector3 startPos_;

	unique_ptr<UpgradeManager> upgradeManager_;

	PlayerName playerName_;

	unique_ptr<AnimationController> model_;
	PlayerMotion currentMotion_ = PlayerMotion::Idle;

	Transform shadowTransform_;

	Transform expGetRangeTransform_;

	unique_ptr<Particles> moveParticle_ = make_unique<Particles>();
	unique_ptr<Particles> landingParticle_ = make_unique<Particles>();
	GameTimer moveParticleTimer_;

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
	
	// ダメージ関連
	GameTimer invincibilityTimer_; // 無敵時間タイマー
	GameTimer blinkTimer_; // 点滅用タイマー
	bool isVisible_ = true; // 描画するかどうか

	Vector3 playerColors[4] = {
	 { 1.000f, 0.447f, 0.133f },
	 { 0.161f, 0.318f, 0.827f },
	 { 0.102f, 0.784f, 0.000f },
	 { 0.239f, 0.855f, 0.922f }
	};
};