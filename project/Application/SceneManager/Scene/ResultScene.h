#pragma once
#include "Application/EngineSystem.h"
#include "Application/SceneManager/IScene.h"
#include <array>

// リザルト画面のモード
namespace Result {
	enum class Screen {
		FIRST,
		SECOND,
	};
}

//using Result::Screen;

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::unique_ptr;

class ResultScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

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
	
	//const float MAX_SCORE = 12340.0f;

	// 桁数
	std::array<int, 5> digits = {0};

	Vector2 testPos_;
	Vector2 testScale_;

	// 集めたお菓子の個数テキストスプライト
	unique_ptr<Sprite> spriteCollectedSweets_ = make_unique<Sprite>();

	// お菓子降ってくる演出用モデル
	std::array<unique_ptr<Model>, 50> sweetModels_;

	// リザルト画面のモード
	Result::Screen currentScreen_ = Result::Screen::FIRST;

	// 「リトライ」テキスト
	unique_ptr<Sprite> spriteRetry_ = make_unique<Sprite>();

	// 「タイトルへ」テキスト
	unique_ptr<Sprite> spriteBackToTitle_ = make_unique<Sprite>();
};
