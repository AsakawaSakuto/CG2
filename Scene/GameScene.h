#pragma once
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
#include "SkyBox.h"
#include "Fade.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class GameScene {
public:
	explicit GameScene(struct AppContext* ctx) : ctx_(ctx) {}

	void Initialize();
	void Update();
	void Draw();
	bool GoTitleScene() { return goTitleScene_; };
	bool GoResultScene() { return goResultScene_; }
	
private:
	AppContext* ctx_ = nullptr;

	bool goTitleScene_ = false; 
	bool goResultScene_ = false;

	GamePad* gamePad_ = nullptr;

	float deltaTime_ = 1.0f / 60.0f;

	unique_ptr<Player> player_ = make_unique<Player>();
	unique_ptr<SkyBox> skyBox_ = make_unique<SkyBox>();

	unique_ptr<Fade> fade_ = make_unique<Fade>();

	unique_ptr<Sprite> pauseBG_ = make_unique<Sprite>();
	unique_ptr<Sprite> pauseUI_ = make_unique<Sprite>();

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
	void DrawFPS_ImGui();
};
