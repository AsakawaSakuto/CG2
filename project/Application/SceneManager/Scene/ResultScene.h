#pragma once
#include "Application/EngineSystem.h"
#include "Application/SceneManager/IScene.h"
#include <array>

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::unique_ptr;

class ResultScene : public IScene {
private:
	// リザルト画面のモード
	enum class Screen {
		FIRST,
		SECOND,
	};

	enum class Menu
	{
		RETRY,
		BACK_TO_TITLE,
	};

public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	~ResultScene();

private:
	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ
	void CameraController();

	// 演出用にスコアを加算していく関数
	void ScoreCountUpdate();

	// スコアに合わせて画像を変更する
	void SpriteScoreUpdate();

	// リソースクリーンアップメソッド
	void CleanupResources();

	// 一枚目のスクリーンの更新
	void ScreenFirstUpdate();

	// 2枚目のスクリーンの更新
	void ScreenSecondUpdate();

	// 一枚目のスクリーンの描画
	void ScreenFirstDraw();

	// 2枚目のスクリーンの描画
	void ScreenSecondDraw();

	// メニューの更新
	void UpdateMenu();

	// ショット数に合わせて画像を変更する
	void SpriteShotCountUpdate();

	// スタン数に合わせて画像を変更する
	void SpriteStunCountUpdate();

	// 背景スプライト
	unique_ptr<Sprite> spriteBG_ = make_unique<Sprite>();

	// スコア表示スプライト
	std::array<unique_ptr<Sprite>, 5> spriteScore_;

	// カウントダウン用のスプライト集
	std::array<std::string, 10> spriteNumCollection_;

	// 演出用の加算スピード
	const float COUNT_SPEED = 0.05f;

	// 描画用のスコア
	float drawScore_ = 0.0f;

	// ゲームシーンから受け取るスコア
	float lastScore_ = 0.0f;

	// ゲームシーンから受け取るショット数
	int lastShotCount_ = 0;

	// ゲームシーンから受け取るスタン数
	int lastStunCount_ = 0;
	
	//const float MAX_SCORE = 12340.0f;

	// 桁数
	std::array<int, 5> digits = {0};

	Vector2 testPos_[2];
	Vector2 testScale_[2];

	// 集めたお菓子の個数テキストスプライト
	unique_ptr<Sprite> spriteCollectedSweets_ = make_unique<Sprite>();

	// お菓子降ってくる演出用モデル
	std::array<unique_ptr<Model>, 50> sweetModels_;

	// リザルト画面のモード
	Screen currentScreen_ = Screen::FIRST;

	// 「リトライ」テキスト
	unique_ptr<Sprite> spriteRetry_ = make_unique<Sprite>();

	// 「タイトルへ」テキスト
	unique_ptr<Sprite> spriteBackToTitle_ = make_unique<Sprite>();

	// カーソルスプライト
	unique_ptr<Sprite> spriteCursol_ = make_unique<Sprite>();

	// 「スタン回数」スプライト
	unique_ptr<Sprite> spriteStunCount_ = make_unique<Sprite>();

	// 「ショット回数」スプライト
	unique_ptr<Sprite> spriteShotCount_ = make_unique<Sprite>();
	
	unique_ptr<Sprite> nextUI_ = make_unique<Sprite>();

	// スクリーン切り替えフラグ
	bool isScreenChange_ = false;

	// 現在のメニュー
	Menu currentMenu_ = Menu::RETRY;

	// 桁数 ショット数、スタン数
	std::array<int, 2> shotCountDigits = {0};
	std::array<int, 2> stunCountDigits = {0};

	// ショット数表示スプライト
	std::array<unique_ptr<Sprite>, 2> spriteShot_;

	// スタン数表示スプライト
	std::array<unique_ptr<Sprite>, 2> spriteStun_;
};
