#pragma once
#include"AppContext.h"

#include"DebugCamera.h"

#include "Player.h"
#include "SkyBox.h"
#include "Fade.h"

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

	float lX_ = 0.0f;
	float lY_ = 0.0f;
	float rX_ = 0.0f;
	float rY_ = 0.0f;

	bool goTitleScene_ = false;

	float timer_ = 0.0f;

	float deltaTime_ = 1.0f / 60.0f;
	unique_ptr<Fade> fade_ = make_unique<Fade>();

	unique_ptr<Player> player_ = make_unique<Player>();
	unique_ptr<SkyBox> skyBox_ = make_unique<SkyBox>();

	unique_ptr<Sprite> pauseBG_ = make_unique<Sprite>();
	unique_ptr<Sprite> pauseUI_ = make_unique<Sprite>();

	unique_ptr<Sprite> testUI_ = make_unique<Sprite>();

	unique_ptr<Object3d> enemy_ = make_unique<Object3d>();

	Vector2 testUIPos_ = { 0.0f,0.0f };

	bool testUIClear_ = false;

	float testUISpeed_ = 128.0f;

	enum Test {
		Test1,
		Test2,
		Test3,
		Test4,
		Test5
	};
	Test testState_ = Test1;

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
	void UpdateTutorialTest();
};