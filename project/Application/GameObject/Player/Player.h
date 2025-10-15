#pragma once
#include "Application/GameObject/GameObject.h"
#include "Bullet.h"
#include <vector>
#include "State/PlayerState.h"
#include "PlayerWing.h"
#include "State/BulletState.h"

class Thorn;
class Block;

struct BulletGaugeInfo {
	std::unique_ptr<Sprite> sprite;
	bool isActive;
};

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

	// ImGui自動生成関数
	void DrawImGuiForJson(nlohmann::json& json);

	// Getter
	Vector3 GetPosition() const { return transform_.translate; }
	float CameraOffset() const { return playerState_.cameraOffset; }
	Direction GetDirection() const { return direction_; }
	Vector2 GetShakeAmount() const { return shakeAmount_; }
	float GetStartLine() const { return START_LINE; }
	float GetEndLine() const { return END_LINE; }
	bool GetIsGoal() const { return isGoal_; }
	int GetBulletGauge() const { return bulletGauge_; }

	// Setter
	void SetThrons(std::vector<std::shared_ptr<Thorn>>& thorns) { thorns_ = thorns; }
	void SetBlocks(std::vector<std::shared_ptr<Block>>& blocks) { blocks_ = blocks; }
	void SetIsGoal(bool isGoal) { isGoal_ = isGoal; }
	void SetBulletGaugeSprites(std::array<BulletGaugeInfo, 5>* gaugeSprites);

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
	void SpeedDown(float speedDpwnStrength);

	// スタン
	void Stun();

	// スタン解除
	void StunRemoved();

	// トゲとの当たり判定
	void CollisionThorn();

	// ブロックとの当たり判定
	void CollisionBlock();

	// 弾とトゲの当たり判定
	void CollisonBulletThorn();

	// カメラシェイクの数値を更新
	void UpdateCameraShake();

	// プレイヤーの羽の状態更新
	void WingStateUpdate();

	// スコア加算
	void AddScore(int score);

	// スコアのImGui
	void ScoreImGui();

	// トゲのクールダウン更新
	void TickThornCooldown();

	// プレイヤーの羽とトゲの当たり判定
	void CollisionWingThorn();

	// 弾のゲージリセット
	void ResetBulletGauge();

	// プレイヤーとの距離に応じて加算するスコアを変化させる
	void AddScoreByDistance(std::shared_ptr<Thorn>& thorn, int scoreAmount);

	// デバッグ用のベクトル表示
	void ShowLabeledVector3(const char* label, float* vec);

	// ImGui自動生成関数
	//void DrawImGuiForJson(nlohmann::json& json);

	// ImGuiプレイヤー
	void DrawImGuiJsonStatePlayer();

	// ImGui弾
	void DrawImGuiJsonStateBullet();

	// 羽のクールダウンフレーム加算
	void WingCoolDownFramesAdd();

	// プレイヤーの移動制限追加
	void PlayerMoveLimit();

private:
	// プレイヤーのStateをJsonで管理
	PlayerState playerState_;
	BulletState bulletState_;

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

	// スコア
	int score_ = 0;

	// 羽関連(スコア)
	const float kNearThreshold = 1.9f;
	const int kNearScore = 1000;
	const int kFarScore = 1;

	float dis{};

	// ゴールフラグ
	bool isGoal_ = false;

	// 現在のクールダウンフレーム
	int currentCoolDownFrames_ = 0;

	// クールダウンの開始フラグ
	bool isStartCoolDown_ = false;

	// 弾のゲージスプライト
	std::array<BulletGaugeInfo, 5>* bulletGaugeSprites_ = nullptr;
};