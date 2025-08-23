#pragma once
#include "IScene.h"

// Include
#include "WinApp.h"
#include "Input.h"
#include "Camera.h"
#include "Audio.h"
#include "DebugCamera.h"
#include "ConvertString.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "TextureManager.h"
#include "GamePad.h"
#include <filesystem>
#include <memory>

// Object
#include "Sprite.h"
#include "Object3d.h"
#include "Sphere.h"
#include "SphereData.h"
#include "Triangle.h"
#include "Particles.h"
#include "EmitterRange.h"
#include "EmitterSpfere.h"

// Math
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include"AppContext.h"

#include "Player.h"
#include "Boss.h"
#include "SkyBox.h"
#include "Fade.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class GameScene : public IScene  {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
private:
	AppContext* ctx_ = nullptr;

	GamePad* gamePad_ = nullptr;

	float deltaTime_ = 1.0f / 60.0f;

	unique_ptr<Player> player_ = make_unique<Player>();
	unique_ptr<Boss> boss_ = make_unique<Boss>();

	unique_ptr<Object3d> skydome_ = make_unique<Object3d>();

	unique_ptr<Fade> fade_ = make_unique<Fade>();

	unique_ptr<Sprite> pauseBG_ = make_unique<Sprite>();
	unique_ptr<Sprite> pauseUI_ = make_unique<Sprite>();

	unique_ptr<Particles> exprotion_ = make_unique<Particles>();
	EmitterSphere exprotionEmitter_ = {};
	EmitterRange exprotionRange_ = {};

	bool isStart = false;
	float startTimer_ = 0.0f;
	const float startTime_ = 5.0f;

	float bossTy_ = 50.0f;
	const float bossTySpeed_ = 10.0f;
	// Cameras
	float cameraRx_ = -0.5f;
	const float cameraRxSpeed_ = 0.1f;
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

	enum State {
		kStart,
		kPlay,
	};

	State state_ = kStart;

	void InitLoad();
	void InitBuilding();
	void UpdateLoad();
	void UpdateBuilding();
	void UpdateCollision();
	void DrawObject();
	const float builSpeed_ = 50.0f;
	const float backLine_ = 0.0f;

	void UpdatePause();
	void UpdateFade();
	void CameraController();
	void DrawFPS_ImGui();

	unique_ptr<Object3d> builA_ = make_unique<Object3d>();
	unique_ptr<Object3d> builB_ = make_unique<Object3d>();
	unique_ptr<Object3d> builC_ = make_unique<Object3d>();
	unique_ptr<Object3d> builD_ = make_unique<Object3d>();
	unique_ptr<Object3d> builE_ = make_unique<Object3d>();
	unique_ptr<Object3d> builF_ = make_unique<Object3d>();
	unique_ptr<Object3d> builG_ = make_unique<Object3d>();
	unique_ptr<Object3d> builH_ = make_unique<Object3d>();
	unique_ptr<Object3d> builI_ = make_unique<Object3d>();
	unique_ptr<Object3d> builJ_ = make_unique<Object3d>();
	unique_ptr<Object3d> builK_ = make_unique<Object3d>();
	unique_ptr<Object3d> builL_ = make_unique<Object3d>();

	unique_ptr<Object3d> builM_ = make_unique<Object3d>();
	unique_ptr<Object3d> builN_ = make_unique<Object3d>();
	unique_ptr<Object3d> builO_ = make_unique<Object3d>();
	unique_ptr<Object3d> builP_ = make_unique<Object3d>();
	unique_ptr<Object3d> builQ_ = make_unique<Object3d>();
	unique_ptr<Object3d> builR_ = make_unique<Object3d>();
	unique_ptr<Object3d> builS_ = make_unique<Object3d>();
	unique_ptr<Object3d> builT_ = make_unique<Object3d>();
	unique_ptr<Object3d> builU_ = make_unique<Object3d>();
	unique_ptr<Object3d> builV_ = make_unique<Object3d>();
	unique_ptr<Object3d> builW_ = make_unique<Object3d>();
	unique_ptr<Object3d> builX_ = make_unique<Object3d>();

	unique_ptr<Object3d> loadA_ = make_unique<Object3d>();
	unique_ptr<Object3d> loadB_ = make_unique<Object3d>();
	unique_ptr<Object3d> loadC_ = make_unique<Object3d>();
	unique_ptr<Object3d> loadD_ = make_unique<Object3d>();
	unique_ptr<Object3d> loadE_ = make_unique<Object3d>();
	unique_ptr<Object3d> loadF_ = make_unique<Object3d>();
	unique_ptr<Object3d> loadEnd_ = make_unique<Object3d>();
};
