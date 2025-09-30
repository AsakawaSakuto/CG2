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
#include "SkyBox.h"
#include "Fade.h"
#include "SceneObject/Ranking/Ranking.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class TestScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
private:
	AppContext* ctx_ = nullptr;

	float deltaTime_ = 1.0f / 60.0f;

	GamePad* gamePad_ = nullptr;

	bool isStart_ = false;
	float startTimer_ = 0.0f;
	bool pModelMove_ = false;

	float quitTimer_ = 0.0f;
	bool isQuit_ = false;

	unique_ptr<Object3d> bunny_ = make_unique<Object3d>();

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = true;

	void CameraController();
	void InitObj();
	void CloseSound();

};
