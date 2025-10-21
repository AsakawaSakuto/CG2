#pragma once
#include "Application/EngineSystem.h"
#include "Application/SceneManager/IScene.h"

#include "Application/GameObject/Player/Player.h"
#include "Application/Map/Map.h"
#include "Application/SceneManager/Scene/State/GameSceneState.h"
#include <Application/GameObject/Block/Block.h>
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

private:
	enum class RuleAnimState { 
		Rising, 
		Waiting, 
		Falling, 
		Done 
	};

	struct SpriteRender
	{
		Sprite sprite;
		bool isDraw;
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
	std::vector<std::shared_ptr<Block>> blocks_;

	// ゲージ用のスプライト(背景)
	unique_ptr<Sprite> bulletGaugeSpriteBG_ = make_unique<Sprite>();

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

	Vector3 testPos_[3] = {};
	Vector3 testScale_[3] = {};

	// プレイヤーから入力があるかどうかでフラグをたてる
	bool isInput_ = false;

	// どのくらいの時間入力が無かったかを記録する変数(秒)
	float noInputTimer_ = 0.0f;

	// ゲームシーンからタイトルに戻るまでの猶予時間(秒)
	// float noInputTimerMax_ = 5.0f;

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

	// ルール説明用のスプライト

	// ルール説明用のスプライトアニメーション制御
	unique_ptr<Sprite> spriteRule_ = make_unique<Sprite>();
	float timerSpriteRule_ = 0.0f;
	float ruleDuration_ = 2.0f;
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

	// ゲーム終了フラグ
	bool isActiveEndText_ = false;
	int timerEndText_ = 0;

	// スコアの背景
	unique_ptr<Sprite> spriteCandyScore_ = make_unique<Sprite>();

	// 弾のゲージラムネ
	unique_ptr<Sprite> spriteChargeUI_ = make_unique<Sprite>();

	// 山のモデル
	std::array<unique_ptr<Model>, 3> modelMountain_;

	// 一定の時間入力がなかった時に減算されるタイマースプライト
	std::array<unique_ptr<Sprite>, 2> spriteNoInputCountDown_;
	
};
