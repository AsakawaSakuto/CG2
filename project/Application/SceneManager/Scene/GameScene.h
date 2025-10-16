#pragma once
#include "Application/EngineSystem.h"
#include "Application/SceneManager/IScene.h"

#include "Application/GameObject/Player/Player.h"
#include "Application/Map/Map.h"
#include <Application/GameObject/Block/Block.h>
#include <Application/GameObject/Thorn/Thorn.h>
#include "Application/SceneManager/Scene/State/GameSceneState.h"

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::unique_ptr;

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

	void SpawnObjectsByMapChip2(float mag, float mapHeight);

	// キーボードやパッドから入力があるかどうか調べる
	void UpdateInput();

	// プレイヤーから一定時間入力がなかった場合の処理
	void NoInputTitleBack();

	// ゲームシーンで管理しているステータスのImGui
	void GameSceneStateImGui();

	// ゲーム開始までのカウント処理
	void GameStartCount();

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

	Vector2 testPos_ = {};
	Vector2 testScale_ = {};

	// プレイヤーから入力があるかどうかでフラグをたてる
	bool isInput_ = false;

	// どのくらいの時間入力が無かったかを記録する変数(秒)
	float noInputTimer_ = 0.0f;

	// ゲームシーンからタイトルに戻るまでの猶予時間(秒)
	//float noInputTimerMax_ = 5.0f;

	// タイトルシーンへの切り替えのフラグ
	bool isBackToTitleScene_ = false;

	// GameSceneState
	GameSceneState gameSceneState_;

	// ゲーム開始までのタイマー
	float gameStartTimer_ = 0.0f;

	// ゲーム開始フラグ
	bool isGameStart_ = false;

	// スプライト
	unique_ptr<Sprite> spriteNumber_ = make_unique<Sprite>();
};
