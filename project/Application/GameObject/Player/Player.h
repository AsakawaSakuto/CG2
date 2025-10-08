#pragma once
#include "Application/GameObject/GameObject.h"
#include "Bullet.h"
#include <vector>

enum class Direction
{
	UP = 0,
	DOWN = 1,
};

class Player : public GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Player() {}

	void SetInputSystem(Input* inInput) { input_ = inInput; }

	// ImGui表示
	void DrawImgui();

	// Getter
	Vector3 GetPosition() const { return transform_.translate; }
	float CameraOffset() const { return cameraOffset_; }
	Direction GetDirection() const { return direction_; }

private:
	// プレイヤーの上昇
	void MovePlayerUpward();

	// プレイヤーのVelocityを一定の値に収める
	void ClampPlayerVelocity();

	// プレイヤーが一定の高さに到達したら反転
	void ReverseIfAboveLimit(float minHeight, float maxHeight);

	// オフセットの変更(補間)
	void CameraOffsetChange();

	// プレイヤーの回転
	void RotateChange();

	// 弾のゲージを貯める
	void BulletCharge();

	// 弾を撃つ
	void BulletShot();

	// 弾の更新処理
	void BulletUpdate();

	// プレイヤーのImGui
	void PlayerImGui();

	// 弾のImGui
	void BulletImGui();

	// 減速
	void SpeedDown();

	// スタン
	void Stun();

	// スタン解除
	void StunRemoved();

private:
	Input* input_ = nullptr;

	// 速度関連
	Vector3 acceleration_{};
	Vector3 velocity_{};

	// オフセット変更フラグ
	bool isOffsetChange_ = false;

	// プレイヤーとカメラのオフセット
	float cameraOffset_ = 4.0f;

	// プレイヤーの進行方向
	Direction direction_ = Direction::UP;

	// 弾のゲージ
	int bulletGauge_ = 0;
	// 弾のゲージ最大数
	const int BULLET_GAUGE_MAX = 5;
	// テスト用の変数
	int num_ = 0;

	// 弾
	std::list<std::unique_ptr<Bullet>> bullets_{};

	// スタン関連
	bool isStun_ = false;
	int stunTimer_ = 0;
	const int kStunDuration = 60; // スタンする時間(フレーム)

	// プレイヤー最高速度
	const float MAX_SPEED = 6.0f;
};