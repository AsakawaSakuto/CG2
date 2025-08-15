#pragma once
#include"AppContext.h"

#include"DebugCamera.h"

#include "Player.h"
#include "SkyBox.h"

class TutorialScene
{
public:

	explicit TutorialScene(struct AppContext* ctx) : ctx_(ctx) {}

	void Initialize();
	void Update();
	void Draw();
	bool GoTitleScene() { return goTitleScene_; }

private:
	AppContext* ctx_ = nullptr;

	GamePad* gamePad_ = nullptr;

	bool goTitleScene_ = false;

	float deltaTime_ = 1.0f / 60.0f;
	bool isFade_ = false;
	float fadeAlpha_ = 1.0f;
	unique_ptr<Sprite> fade_ = make_unique<Sprite>();

	unique_ptr<Player> player_ = make_unique<Player>();
	unique_ptr<SkyBox> skyBox_ = make_unique<SkyBox>();

	unique_ptr<Sprite> pauseBG_ = make_unique<Sprite>();
	unique_ptr<Sprite> pauseUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> loadingUI_ = make_unique<Sprite>();

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = false;

	bool isPause_ = false;

	enum Pause {
		kBack,
		kQuit
	};

	Pause pause_ = kBack;

	void UpdatePause();
	void UpdateFade();
	void CameraController();
};