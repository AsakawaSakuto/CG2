#pragma once
#include "Application/EngineSystem.h"
#include "Application/SceneManager/IScene.h"

#include "Application/GameObject/Player/Player.h"
#include "Application/Map/Map.h"
#include "Application/SceneManager/Scene/State/GameSceneState.h"
#include <Application/GameObject/Thorn/Thorn.h>

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::unique_ptr;

class SceneManager;

class GameScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	~GameScene();

private:
	// カメラの座標Xをプレイヤーに合わせる
	void UpdateCameraToPlayer();

	// マップチップに基づいてオブジェクトを配置する
	void SpawnObjectsByMapChip(float mag, float mapHeight);

	// キーボードやパッドから入力があるかどうか調べる
	void UpdateInput();

	// プレイヤーから一定時間入力がなかった場合の処理
	void NoInputTitleBack();

	// ゲームシーンで管理しているステータスのImGui
	void GameSceneStateImGui();

	// ゲーム開始までのカウント処理
	void GameStartCount();

	// 演出用にスコアを加算していく関数
	void SpriteScoreUpdate();

	// ルール説明用のスプライトアニメーション
	void AnimationRuleSprite();

	// プレイヤーの進行度スプライト移動処理
	void UpdateProgressSprite();

	// 終了テキストの更新
	void UpdateEndText();

	// オーディオの更新
	void AudioUpdate();

	// ○○個突破スコアの更新
	void UpdateSpriteScoreOver();

	// ○○個突破!アニメーション
	void AnimationSpriteSnackOver();

	// アニメーション開始関数
	void StartSnackOverAnimation();

	// カメラの調整ImGui
	void CameraStateImGui();

	// Pushスプライト切り替え
	void PushSpriteChange();

	// クマのスプライトの回転
	void UpdateSpriteRotation();

	// ラムネの波紋
	void UpdateSpriteChargeEffect();

	// スコアの背景波紋
	void UpdateSpriteCandyEffect();

	// SEの解放
	void ResetSE();

	// スコア増加時のアニメーション
	void ScoreUpAnimation();

	// ルール説明用のスプライト回転更新
	void UpdateRuleSprite();

	// 二つの値からランダムに値を返す
	float RandomFloat(float min, float max);

	// 雲のモデルの挙動
	void UpdateCloudModel();

	// Directionをランダムに返す
	int GetRandomDirection();

	// 雲の座標リセット
	void ResetCloudPos();

private:
	enum class RuleAnimState { Rising, Waiting, Falling, Done };

	enum class RuleSpriteState {
		RIGHT,
		LEFT,
	};

	enum Direction {
		RIGHT,
		LEFT,
	};

	struct SpriteRender {
		Sprite sprite;
		bool isDraw;
	};

	struct MoveModel {
		Model model;
		Direction direction;
	};

private:
	int goSceneNum_ = 0; // 0ならタイトル、1ならリザルト

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = false;                                      // デバッグカメラ使用フラグ
	void CameraController();

	// AppContext
	AppContext* ctx_ = nullptr;

	// InputSystem
	GamePad* gamePad_ = nullptr;
	Input* input_ = nullptr;

	// Objects
	unique_ptr<Model> model_ = make_unique<Model>();
	unique_ptr<Sprite> sprite_ = make_unique<Sprite>();
	unique_ptr<Player> player_ = make_unique<Player>();
	std::vector<std::shared_ptr<Thorn>> thorns_;

	// ゲージ用のスプライト
	std::array<BulletGaugeInfo, 5> bulletGaugeSprite_;

	// 画面両端の幕のスプライト
	std::array<std::unique_ptr<Sprite>, 2> curtainSprite_;

	// UtilSystem
	GameTimer gameTimer_;

	// Map
	unique_ptr<Map> map_ = make_unique<Map>();

	//
	Vector3 startPos_ = {0.0f, 0.0f, 0.0f};
	Vector3 endPos_ = {10.0f, 0.0f, 0.0f};

	// プレイヤーがゴールしたかどうか
	bool isGoal_ = false;

	// プレイヤーから入力があるかどうかでフラグをたてる
	bool isInput_ = false;

	// どのくらいの時間入力が無かったかを記録する変数(秒)
	float noInputTimer_ = 0.0f;

	// タイトルシーンへの切り替えのフラグ
	bool isBackToTitleScene_ = false;

	// GameSceneState
	GameSceneState gameSceneState_;

	// ゲーム開始までのタイマー
	float gameStartTimer_ = 0.0f;

	// ゲーム開始フラグ
	bool isGameStart_ = false;

	// 開始カウントダウンスプライト
	unique_ptr<Sprite> spriteNumber_ = make_unique<Sprite>();

	// カウントダウン用のスプライト集
	std::array<std::string, 10> spriteNumCollection_;

	// 桁数
	std::array<int, 5> digits = {0};

	// スコア表示スプライト
	std::array<unique_ptr<SpriteRender>, 5> spriteScore_;

	// 「スタート!」スプライト
	unique_ptr<Sprite> spriteStart_ = make_unique<Sprite>();

	// Startスプライト用アニメーション制御
	bool showStart_ = false;
	float startAnimTimer_ = 0.0f;
	float startAnimDuration_ = 1.0f;
	Vector2 startAnimStartScale_ = {0.5f, 0.5f};
	Vector2 startAnimEndScale_ = {1.5f, 1.5f};
	float startAnimStartAlpha_ = 1.0f;
	float startAnimEndAlpha_ = 0.0f;

	// ルール説明用のスプライトアニメーション制御
	unique_ptr<Sprite> spriteRule_ = make_unique<Sprite>();
	float timerSpriteRule_ = 0.0f;
	float ruleDuration_ = 0.75f;
	float ruleWaitDuration_ = 0.5f;
	float ruleStartPosY_ = -100.0f;
	float ruleEndPosY_ = 100.0f;
	RuleAnimState ruleState_ = RuleAnimState::Rising;
	bool ruleLoop_ = false;

	// ゲーム終了時に表示するスプライト
	unique_ptr<Sprite> spriteGameEnd_ = make_unique<Sprite>();

	// 進行度ゲージスプライト
	unique_ptr<Sprite> spriteProgressLine_ = make_unique<Sprite>();
	unique_ptr<Sprite> spriteProgressPlayer_ = make_unique<Sprite>();
	unique_ptr<Sprite> spriteProgressGoal_ = make_unique<Sprite>();
	unique_ptr<Sprite> spriteProgressMountaion_ = make_unique<Sprite>();

	// ゲーム終了フラグ
	bool isActiveEndText_ = false;
	int timerEndText_ = 0;

	// スコアの背景
	unique_ptr<Sprite> spriteCandyScore_ = make_unique<Sprite>();

	// スコアの背景波紋
	unique_ptr<Sprite> spriteCandyEffect_ = make_unique<Sprite>();
	float candyEffectSize_ = 1.0f;                                                 // サイズ
	float candyEffectAlpha_ = 1.0f;                                                // 透明度
	std::array<float, 5> candySizeSpeeds = {0.0025f, 0.005f, 0.01f, 0.02f, 0.04f}; // サイズの変化スピード

	// 弾のゲージラムネ
	unique_ptr<Sprite> spriteChargeUI_ = make_unique<Sprite>();

	// ラムネの波紋
	unique_ptr<Sprite> spriteChargeUIEffect_ = make_unique<Sprite>();
	float chargeEffectSize_ = 0.5f;                                             // サイズ
	float chargeEffectAlpha_ = 1.0f;                                            // 透明度
	std::array<float, 5> gaugeSizeSpeeds = {0.01f, 0.02f, 0.03f, 0.04f, 0.06f}; // サイズの変化スピード

	// 山のモデル
	std::array<unique_ptr<Model>, 3> modelMountain_;

	// 一定の時間入力がなかった時に減算されるタイマースプライト
	std::array<unique_ptr<Sprite>, 2> spriteNoInputCountDown_;

	// SE
	unique_ptr<AudioX> startGameSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> countDownSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> clearSE_ = make_unique<AudioX>();
	// BGM
	unique_ptr<AudioX> gameSceneBGM01_ = make_unique<AudioX>();
	unique_ptr<AudioX> gameSceneBGM02_ = make_unique<AudioX>();

	float startGameSE_BaseVolume_ = 0.45f;
	float countDownSE_BaseVolume_ = 0.65f;
	float clearSE_BaseVolume_ = 0.8f;
	float gameSceneBGM01_BaseVolume_ = 1.0f;
	float gameSceneBGM02_BaseVolume_ = 0.5f;

	// ○○個突破 スプライト
	unique_ptr<Sprite> spriteSnackCountOver_ = make_unique<Sprite>();

	// 前回チェックしたスコア
	float lastScoreChecked_ = 0.0f;

	// アニメーションの状態
	RuleAnimState snackCountOverAnimationState_ = RuleAnimState::Done;

	// アニメーション用タイマー
	float timerSnackCountOver_ = 0.0f;

	// ○○個突破 スコア数
	std::array<unique_ptr<Sprite>, 5> spriteScoreCountOver_;

	// ○○個突破 スコア数 座標
	Vector2 spriteScoreCountOverPos_ = {-600.0f, 640.0f};

	// ○○個突破　1000の倍数
	float nextMilestone_ = 0;

	// ○○個突破 イージング用
	const float kScoreOverStartPosX_ = -600.0f;
	const float kScoreOverMiddlePosX_ = 640.0f;
	const float kScoreOverEndPosX_ = 1880.0f;
	const float kScoreOverWaitDuration_ = 0.1f;

	// カメラのZ座標
	float cameraPosisionZ_ = -30.0f;

	// push スプライト
	unique_ptr<Sprite> spritePush_ = make_unique<Sprite>();

	// pushスプライト切り替え用のタイマー
	int pushSpriteTimer_ = 0;

	// 腕 スプライト
	unique_ptr<Sprite> spriteArm_ = make_unique<Sprite>();

	// ピニャータ スプライト
	unique_ptr<Sprite> spriteThorn_ = make_unique<Sprite>();

	// クマのスプライト回転用の変数
	float frameCount_ = 0.0f;

	// スコア増加時のアニメーション用変数
	float scoreUpTimer_ = 0.0f;
	float scoreUpDuration_ = 0.2f;

	// ルール説明用のスプライト角度
	float rotateRuleSprite_ = 0.0f;

	// 回転更新用タイマー
	int rotateRuleTimer_ = 0;

	// ルール説明用のスプライト状態
	RuleSpriteState currentRuleSpriteState_ = RuleSpriteState::RIGHT;

	// 地面モデル
	unique_ptr<Model> modelGround_ = make_unique<Model>();

	// 曇
	std::array<std::unique_ptr<MoveModel>, 30> clouds_;

	// 雲のモデルリセットフラグ
	bool isCloudPosReset_ = false;

	// mask
	unique_ptr<Sprite> mask_ = make_unique<Sprite>();
	unique_ptr<Sprite> maskBox_ = make_unique<Sprite>();
	unique_ptr<Sprite> loadingUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> loadingPlayer_ = make_unique<Sprite>();
	Vector2 maskStartPos_;
	Vector2 maskEndPos_;
	Vector2 maskStartScale_;
	Vector2 maskEndScale_;

	enum class MaskType {
		RAMA = 0,
		KUMA = 1,
		AME = 2,
	};

	MaskType maskType_ = MaskType::AME;

	Random rand_;

	GameTimer maskTimer_;

	bool timerStarte_ = false;
	bool resultQuit_ = false;

	unique_ptr<Model> ore_ = make_unique<Model>();
	Transform oreTransform_;

	unique_ptr<Particles> srarArea1_ = make_unique<Particles>();
	unique_ptr<Particles> srarArea2_ = make_unique<Particles>();
	unique_ptr<Particles> srarArea3_ = make_unique<Particles>();
	unique_ptr<Particles> srarArea4_ = make_unique<Particles>();
};
