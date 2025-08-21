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

	unique_ptr<Fade> fade_ = make_unique<Fade>();

	unique_ptr<Sprite> reticle2D_ = make_unique<Sprite>();

	unique_ptr<Object3d> yuka_ = make_unique<Object3d>();
	unique_ptr<Object3d> player1_ = make_unique<Object3d>();
	unique_ptr<Object3d> player2_ = make_unique<Object3d>();

	// ---------- magic ---------- //
	unique_ptr<Particles> fire_ = make_unique<Particles>();
	EmitterSphere fireEmitter_ = {};
	EmitterRange fireRange_ = {};
	Vector3 firePos_ = {};
	Vector3 fireVelocity_ = {};
	bool fireIsAlive_ = false;
	const float fireSpeed_ = 10.0f;
	const float fireLifeTime_ = 5.0f;
	float fireLifeTimer_ = 0.0f;

	const float kDistanceToReticle = 60.0f;

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = false;

	void InitParticle();
	void CameraController();
	void DrawFPS_ImGui();
};
