#pragma once
#include"IScene.h"
#include"AppContext.h"
#include"DebugCamera.h"

#include "Player.h"
#include "SkyBox.h"
#include "Fade.h"
#include "Audio.h"

class TutorialScene : public IScene
{
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
private:

	AppContext* ctx_ = nullptr;

	GamePad* gamePad_ = nullptr;

	unique_ptr<Fade> fade_ = make_unique<Fade>();

	unique_ptr<Player> player_ = make_unique<Player>();
	unique_ptr<SkyBox> skyBox_ = make_unique<SkyBox>();

	unique_ptr<Sprite> pauseBG_ = make_unique<Sprite>();
	unique_ptr<Sprite> pauseUI_ = make_unique<Sprite>();

	unique_ptr<Sprite> deviceUI_ = make_unique<Sprite>();

	unique_ptr<Sprite> testUI_ = make_unique<Sprite>();

	unique_ptr<Object3d> enemy_ = make_unique<Object3d>();
	unique_ptr<Object3d> enemyBullet_ = make_unique<Object3d>();

	float lX_ = 0.0f;
	float lY_ = 0.0f;
	float rX_ = 0.0f;
	float rY_ = 0.0f;

	float timer_ = 0.0f;

	float deltaTime_ = 1.0f / 60.0f;

	Vector2 testUIPos_ = { 0.0f,0.0f };

	bool testUIClear_ = false;

	float testUISpeed_ = 128.0f;

	enum Test {
		Test1,
		Test2,
		Test3,
		Test4,
		Test5,
		Test6
	};
	Test testState_ = Test1;

	// enemyValue
	bool isUpDownMove_ = false;
	bool isAttack_ = false;
	bool isAlive_ = false;
	float isAttackTimer_ = 0.0f;
	float upDownSpeed_ = 2.0f;
	float zRotateSpeed_ = 3.0f;
	float bulletSpeed_ = 75.0f;
	Vector3 bulletVelocity_ = {};

	unique_ptr<Particles> exprotion_ = make_unique<Particles>();
	EmitterSphere exprotionEmitter_ = {};
	EmitterRange exprotionRange_ = {};

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

	unique_ptr<AudioX> selectSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> pushSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> quitSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> pauseSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> clearSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> spinSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> shotSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> bgm_ = make_unique<AudioX>();

	void UpdatePause();
	void UpdateFade();
	void UpdateEnemy();
	void UpdateCollision();
	void UpdateTutorialTest();
	void CameraController();
	void CloseSound();
};