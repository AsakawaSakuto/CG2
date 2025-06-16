#pragma once

// Includes
#include "WinApp.h"
#include "Input.h"
#include "Camera.h"
#include "Audio.h"
#include "DebugCamera.h"
#include "ConvertString.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "TextureManager.h"
#include <filesystem>
#include <memory> // ← 追加
// Object
#include "Sprite.h"
#include "SpriteData.h"
#include "Object3d.h"
#include "Sphere.h"
#include "SphereData.h"
#include "Triangle.h"
#include "Particles.h"
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
	MSG msg{};

	// Core systems
	unique_ptr<WinApp> winApp = make_unique<WinApp>();
	unique_ptr<DirectXCommon> dxCommon = make_unique<DirectXCommon>();
	D3ResourceLeakChecker d3ResourceLeakCheker;
	unique_ptr<Input> input = make_unique<Input>();

	// Sprites
	//unique_ptr<SpriteData> spriteData = make_unique<SpriteData>();
	//unique_ptr<Sprite> sprite = make_unique<Sprite>();

	// 3D Objects
	unique_ptr<Object3d> model = make_unique<Object3d>();
	unique_ptr<Object3d> model2 = make_unique<Object3d>();
	unique_ptr<Object3d> skydome = make_unique<Object3d>();
	unique_ptr<Object3d> fenceModel = make_unique<Object3d>();

	// Audio
	unique_ptr<Audio> audio = make_unique<Audio>();
	unique_ptr<Audio> audio2 = make_unique<Audio>();

	// Other render objects
	//unique_ptr<SphereData> sphereData = make_unique<SphereData>();
	//unique_ptr<Sphere> sphere = make_unique<Sphere>();
	//unique_ptr<Triangle> triangle = make_unique<Triangle>();
	unique_ptr<Particles> particles = make_unique<Particles>();

	// Cameras
	unique_ptr<Camera> camera = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera = make_unique<DebugCamera>();
	Camera* useCamera = nullptr;
	bool isDebugCamera = true;

	// Option flags
	bool modelTexture = true;
	bool particlesTexture = true;
};