#pragma once
#include "Application/GameObject/GameObject.h"
#include "Bullet.h"
#include <vector>
#include "State/PlayerState.h"
#include "PlayerWing.h"

class Thorn;
class Block;

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
	void SetGamePadSystem(GamePad* inGamePad) { gamePad_ = inGamePad; }

	// ImGui表示
	void DrawImgui();

	void DrawImGuiJsonState();

	// Getter
	Vector3 GetPosition() const { return transform_.translate; }
	float CameraOffset() const { return state_.cameraOffset; }
	Direction GetDirection() const { return direction_; }
	Vector2 GetShakeAmount() const { return shakeAmount_; }
	float GetStartLine() const { return START_LINE; }
	float GetEndLine() const { return END_LINE; }

	// Setter
	void SetThrons(std::vector<std::shared_ptr<Thorn>>& thorns) { thorns_ = thorns; }
	void SetBlocks(std::vector<std::shared_ptr<Block>>& blocks) { blocks_ = blocks; }

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

	// トゲとの当たり判定
	void ThornCollision();

	// ブロックとの当たり判定
	void BlockCollision();

	// 弾とトゲの当たり判定
	void BulletThornCollison();

	// カメラシェイクの数値を更新
	void UpdateCameraShake();

	// プレイヤーの羽の状態更新
	void WingStateUpdate();

private:
	// プレイヤーのStateをJsonで管理
	PlayerState state_;

	Input* input_ = nullptr;
	GamePad* gamePad_ = nullptr;

	// 速度関連
	Vector3 acceleration_{};
	Vector3 velocity_{};

	// プレイヤーの進行方向
	Direction direction_ = Direction::UP;

	// 弾のゲージ
	int bulletGauge_ = 0;
	// テスト用の変数
	int num_ = 0;

	// 弾
	std::list<std::unique_ptr<Bullet>> bullets_{};

	// スタン関連
	bool isStun_ = false;

	GameTimer stunTimer_;

	// トゲ
	std::vector<std::shared_ptr<Thorn>> thorns_;

	// ブロック
	std::vector<std::shared_ptr<Block>> blocks_;

	// プレイヤーの羽
	std::unique_ptr<PlayerWing> playerWing_ = std::make_unique<PlayerWing>();

	// スタートライン、最終ライン
	const float START_LINE = -100.0f;
	const float END_LINE = 100.0f;

	// カメラオフセット
	const float CAMERA_OFFSET_TOP = 4.0f;
	const float CAMERA_OFFSET_BOTTOM = -4.0f;

	// シェイク関連
	bool isShake_ = false;
	Vector2 shakeAmount_ = {0.0f, 0.0f};
};