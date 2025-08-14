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

class GameScene {
public:
	explicit GameScene(struct AppContext* ctx) : ctx_(ctx) {}

	void Initialize();
	void Finalize();
	void Update();
	void Draw();
	bool SceneChange() { return sceneChange_; }
	bool IsEndRequst() { return endRequst_; }
	
private:
	AppContext* ctx_ = nullptr;

	bool endRequst_ = false;
	bool sceneChange_ = false;

	GamePad* gamePad_ = nullptr;

	unique_ptr<Player> player_ = make_unique<Player>();
	unique_ptr<SkyBox> skyBox_ = make_unique<SkyBox>();

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = false;

	void CameraController();
	void DrawFPS_ImGui();
};
