#pragma once
#include "GameObject/BaseGameObject.h"
#include "EngineSystem.h"
#include "Utility/Collision/Type/AABB.h"

class Map3D; // 前方宣言

class Enemy : public BaseGameObject {
public:
    void Initialize() override;
    void Update() override;
    void Draw(Camera camera) override;
    void DrawImGui() override;
    ~Enemy() override = default; // デストラクタを追加（unique_ptrの確実な解放を保証）

	void SetPosition(const Vector3& position) { transform_.translate = position; }

	void SetTargetPosition(const Vector3& target) { targetPosition_ = target; }
	void PushAway(const Vector3& otherPosition, float otherRadius);
	
	// Map3Dを設定
	void SetMap(Map3D* map) { map_ = map; }

private:

    void Move();
	
	// マップとの衝突解決
	void ResolveMapCollision();
	
	// マップ上で地面に接しているかチェック
	bool IsGroundedOnMap();

private:
    unique_ptr<SkiningModel> model_ = make_unique<SkiningModel>();
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };
	float moveSpeed_ = 2.0f; // デフォルトの移動速度
	float collicionRadius_ = 0.5f;

	GameTimer scaleTimer_;
	
	// Map3Dへの参照
	Map3D* map_ = nullptr;
	
	// マップコリジョン用AABB
	AABB mapCollosion_;
	
	// 物理関連
	float velocity_Y = 0.0f;       // 垂直速度
	float gravity_ = 20.0f;        // 重力
	bool isGrounded_ = false;      // 地面に接しているか
	
	// 壁に沿って上昇する速度
	float wallClimbSpeed_ = 15.0f;
	
	// 移動方向（壁上昇判定用）
	Vector3 moveDirection_ = { 0.0f, 0.0f, 0.0f };
};