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

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class GameScene {
public:

	void Initialize();
	void Finalize();
	void Update();
	void Draw();
	bool IsEndRequst() { return endRequst_; }

private:
	bool endRequst_ = false;
	MSG msg_{};

	GamePad gamePad_;

	// Core systems
	unique_ptr<WinApp> winApp_ = make_unique<WinApp>();
	unique_ptr<DirectXCommon> dxCommon_ = make_unique<DirectXCommon>();
	unique_ptr<Input> input_ = make_unique<Input>();
	D3ResourceLeakChecker d3ResourceLeakCheker_;

	//Sprite
	unique_ptr<Sprite> sprite_ = make_unique<Sprite>();

	// 3D Object
	unique_ptr<Object3d> skydome_ = make_unique<Object3d>();
	unique_ptr<Object3d> player_ = make_unique<Object3d>();

	// Audio
	unique_ptr<Audio> audio_ = make_unique<Audio>();
	unique_ptr<Audio> audio2_ = make_unique<Audio>();

	// Other render objects
	unique_ptr<Particles> particle_ = make_unique<Particles>();
	unique_ptr<Particles> particle2_ = make_unique<Particles>();

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = true;

	void CameraController();
	void DrawFPS_ImGui();
};
