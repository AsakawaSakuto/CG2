#pragma once
#include "Application/GameObject/GameObject.h"
#include "Application/GameObject/Player/Particle/stunP.h"
#include "Application/Score/ScoreList.h"
#include "Bear.h"
#include "Bullet.h"
#include "PlayerWing.h"
#include "State/BulletState.h"
#include "State/PlayerState.h"
#include <vector>

class Thorn;
class Block;

struct BulletGaugeInfo {
	std::unique_ptr<Sprite> sprite;
	bool isActive;
};

struct CameraShakeParams {
	float strength;
	float duration;
	float decayRate;
};

enum class PlayerDirection {
	UP = 0,
	DOWN = 1,
};

enum class ShakeType {
	HitByEnemy,  // 被弾時
	AttackEnemy, // 敵に攻撃を与えた時
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
	void DrawImGuiForJson(nlohmann::json& json, float changeFloat);

	// Getter
	Vector3 GetPosition() const { return transform_.translate; }
	float CameraOffset() const { return playerState_.cameraOffset; }
	PlayerDirection GetDirection() const { return direction_; }
	Vector2 GetShakeAmount() const { return shakeAmount_; }
	float GetStartLine() const { return START_LINE; }
	float GetEndLine() const { return END_LINE; }
	bool GetIsGoal() const { return isGoal_; }
	int GetBulletGauge() const { return bulletGauge_; }
	float GetScore() const { return score_; }
	int GetShotCount() const { return shotCount_; }
	int GetStunCount() const { return stunCount_; }
	bool GetIsCameraSet() const { return isCameraSet_; }
	bool GetIsScoreUpAnimation() const { return isScoreUpAnimation_; }

	// Setter
	void SetThrons(std::vector<std::shared_ptr<Thorn>>& thorns) { thorns_ = thorns; }
	void SetBlocks(std::vector<std::shared_ptr<Block>>& blocks) { blocks_ = blocks; }
	void SetIsGoal(bool isGoal) { isGoal_ = isGoal; }
	void SetBulletGaugeSprites(std::array<BulletGaugeInfo, 5>* gaugeSprites);
	void SetIsScoreUpAnimation(bool isScoreUpAnimation) { isScoreUpAnimation_ = isScoreUpAnimation; }
	void SetIsCountDownZero(bool isCountDownZero) { isCountDownZero_ = isCountDownZero; }

	void AudioReset();
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

	// 減速
	void SpeedDown(float speedDpwnStrength);

	// スタン
	void Stun();

	// スタン解除
	void StunRemoved();

	// トゲとの当たり判定
	void CollisionThorn();

	// 弾とトゲの当たり判定
	void CollisionBulletThorn();

	// カメラシェイクの数値を更新
	void UpdateCameraShake();

	// プレイヤーの羽の状態更新
	void WingStateUpdate();

	// スコア加算
	void AddScore(float score);

	// トゲのクールダウン更新
	void TickThornCooldown();

	// プレイヤーの羽とトゲの当たり判定
	void CollisionWingThorn();

	// 弾のゲージをセット
	void SetBulletGauge(int point);

	// プレイヤーとの距離に応じて加算するスコアを変化させる
	void AddScoreByDistance(std::shared_ptr<Thorn>& thorn, float scoreAmount);

	// デバッグ用のベクトル表示
	void ShowLabeledVector3(const char* label, float* vec);

	// ImGui自動生成関数
	// void DrawImGuiForJson(nlohmann::json& json);

	// ImGuiプレイヤー
	void DrawImGuiJsonStatePlayer();

	// ImGui弾
	void DrawImGuiJsonStateBullet();

	// 羽のクールダウンフレーム加算
	void WingCoolDownFramesAdd();

	// プレイヤーの移動制限追加
	void PlayerMoveLimit();

	// プレイヤーの当たり判定更新(AABB)
	void UpdateCollisionAABB();

	// プレイヤーがスタン時に回転する
	void StunRotate();

	// カメラの追従オンオフ切り替え
	void UpdateCameraSetChange();

	// プレイヤーの左右移動
	void UpdatePlayerHorizontalMove();

	// オーディオの更新
	void AudioUpdate();

	// ImGuiスコア
	void DrawImGuiJsonStateScore();

	void InitParticle();

	void UpdateParticle();

	void DrawParticle(Camera useCamera);

	// 点滅用の更新
	void UpdateFlicker();

	// シェイクの開始関数
	void StartCameraShake(ShakeType type);

	// 前のフレームよりスコアが増えていたらアニメーションする
	void ComparisonScore();

	// 加算するスコアを受け取ってpushする
	void ScoreParticleAdd(float score);

	// スコア加算時のパーティクル更新
	void ScoreParticleAddUpdate();

	void TranslateLerp();
private:
	// プレイヤーのStateをJsonで管理
	PlayerState playerState_;
	BulletState bulletState_;
	ScoreList scoreList_;

	Input* input_ = nullptr;
	GamePad* gamePad_ = nullptr;

	// 速度関連
	Vector3 acceleration_{};
	Vector3 velocity_{};

	// プレイヤーの進行方向
	PlayerDirection direction_ = PlayerDirection::UP;

	// 弾のゲージ
	int bulletGauge_ = 0;
	// テスト用の変数
	int num_ = 0;

	// 弾
	std::list<std::unique_ptr<Bullet>> bullets_{};

	// スタン関連
	GameTimer stunTimer_;

	// トゲ
	std::vector<std::shared_ptr<Thorn>> thorns_;

	// ブロック
	std::vector<std::shared_ptr<Block>> blocks_;

	// プレイヤーの羽
	std::unique_ptr<PlayerWing> playerWing_ = std::make_unique<PlayerWing>();

	// スタートライン、最終ライン
	const float START_LINE = -10.0f;
	const float END_LINE = 440.0f;

	// ゲーム終了用のライン
	const float GAME_END_LINE = 20.0f;

	// カメラオフセット
	const float CAMERA_OFFSET_TOP = 5.0f;
	const float CAMERA_OFFSET_BOTTOM = -5.0f;

	// シェイク関連
	bool isShake_ = false;
	Vector2 shakeAmount_ = {0.0f, 0.0f};

	// スコア
	float score_ = 0;

	// 羽関連(スコア)
	float kNearThreshold = 2.0f;

	float dis{};

	// ゴールフラグ
	bool isGoal_ = false;

	// 現在のクールダウンフレーム
	int currentCoolDownFrames_ = 0;

	// クールダウンの開始フラグ
	bool isStartCoolDown_ = false;

	// 弾のゲージスプライト
	std::array<BulletGaugeInfo, 5>* bulletGaugeSprites_ = nullptr;

	// ショット数のカウント
	int shotCount_ = 0;

	// ショット数のカウント
	int stunCount_ = 0;

	// クマ
	std::unique_ptr<Bear> bear_ = std::make_unique<Bear>();

	// プレイヤーにカメラが追従するかどうかのフラグ
	bool isCameraSet_ = false;

	// 点滅用の変数・定数
	bool isFlicker_ = false;
	int currentFlickFrames_ = 0;
	const int kMaxFrameFlick = 60;

	// SE
	unique_ptr<AudioX> shotSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> playerDamageSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> DestroyEnemySE_ = make_unique<AudioX>();
	unique_ptr<AudioX> gaugeChargeSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> getItemSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> attackEnemySE_ = make_unique<AudioX>();

	// ramuneから出るパーティクル
	std::unique_ptr<Particles> ramuneParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> ramuneWhiteParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> kasokuParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> smorkParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> bulletChargeParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> bulletShotParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> bulletDieParticle_ = std::make_unique<Particles>();
	// BulletMoveParticleを5個用意
	std::array<std::unique_ptr<Particles>, 5> bulletMoveParticles_;
	std::unique_ptr<Particles> stateChangeParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> fallParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> armHitParticle1_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> armHitParticle2_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> armHitParticle3_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> goalParticle1_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> goalParticle2_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> getScoreParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> boostParticle_ = std::make_unique<Particles>();
	std::unique_ptr<Particles> boost2Particle_ = std::make_unique<Particles>();

	GameTimer boostTimer_;

	float ramuneOffsetY_ = -3.5f;
	float kasokuOffsetY_ = 12.0f;
	std::unique_ptr<StunP> stunParticle_ = std::make_unique<StunP>();
	GameTimer stateChangeTimer_;
	bool playerIsMove_ = true;

	// シェイクの種類
	const std::unordered_map<ShakeType, CameraShakeParams> kShakePresets = {
	    {ShakeType::HitByEnemy,  {1.5f, 0.3f, 5.0f}},
        {ShakeType::AttackEnemy, {0.4f, 0.2f, 3.0f}}
    };

	// シェイク用の変数
	float shakeTimer_ = 0.0f;
	float shakeDecayRate_ = 0.0f;

	// 弾数に応じたプレイヤーの最高速度
	std::array<const float, 6> playerMaxSpeeds_ = {10.0f, 12.0f, 14.0f, 15.0f, 16.0f, 17.0f};

	// プレイヤーの移動制限用の変数
	const float moveLimitPosX_ = 7.0f;

	// スコア増加時のアニメーション用変数
	bool isScoreUpAnimation_ = false;

	// 前フレームのスコア
	float preScore_ = 0.0f;

	// カウントダウンが0になったかどうか
	bool isCountDownZero_ = false;

	// 加算スコアパーティクル
	std::vector<std::vector<std::unique_ptr<Sprite>>> spriteAddScoreParticle_;

	// 数字スプライト集
	std::array<std::string, 10> spriteNumCollection_;

	// 弾の溜まる速度
	const int chrageSpeed_ = 90;
};