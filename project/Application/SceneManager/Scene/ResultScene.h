#pragma once
#include "Application/EngineSystem.h"
#include "Application/SceneManager/IScene.h"
#include <array>

#include "Application/GameObject/ResultObject/Score.h"

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::unique_ptr;

class ResultScene : public IScene {
private:

public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	~ResultScene();

private:

	enum class MaskType {
		RAMA = 0,
		KUMA = 1,
		AME = 2,
	};

	MaskType maskType_ = MaskType::AME;

	int nextScene_ = 0;

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = false;                                       // デバッグカメラ使用フラグ
	void CameraController();

	float lastScore_ = 0.0f; // 前のシーンから受け取るスコア

	unique_ptr<Score> score_ = make_unique<Score>();

	// mask
	unique_ptr<Sprite> mask_ = make_unique<Sprite>();
	unique_ptr<Sprite> maskBox_ = make_unique<Sprite>();
	unique_ptr<Sprite> loadingUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> loadingPlayer_ = make_unique<Sprite>();
	Vector2 maskStartPos_;
	Vector2 maskEndPos_;
	Vector2 maskStartScale_;
	Vector2 maskEndScale_;

	GameTimer maskTimer_;
	GameTimer quitTimer_;

	bool timerStarte_ = false;
	bool resultQuit_ = false;

	Random rand_;
};
