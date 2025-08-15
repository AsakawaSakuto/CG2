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

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class TitleScene {
public:
	explicit TitleScene(struct AppContext* ctx) : ctx_(ctx) {}

	void Initialize();
	void Finalize();
	void Update();
	void Draw();
	bool GoGameScene() { return goGameScene_; };
	bool GoTutorialScene() { return goTutorialScene_; }
	bool GoQuit() { return goQuit_; }

private:
	AppContext* ctx_ = nullptr;

	bool goGameScene_ = false;
	bool goTutorialScene_ = false;
	bool goQuit_ = false;

	float deltaTime_ = 1.0f / 60.0f;

	bool isFade_ = false;
	float fadeAlpha_ = 0.0f;

	GamePad* gamePad_ = nullptr;

	unique_ptr<Sprite> test_ = make_unique<Sprite>();
	unique_ptr<Sprite> titleUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> fade_ = make_unique<Sprite>();

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = false;

	void CameraController();

	enum State {
		kPlay,
		kTutorial,
		kQuit,
	};

	State state_ = kPlay;
};
