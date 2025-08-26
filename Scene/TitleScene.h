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

class TitleScene : public IScene {
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

	unique_ptr<AudioX> test_ = make_unique<AudioX>();
	unique_ptr<AudioX> test2_ = make_unique<AudioX>();

	unique_ptr<Sprite> titleUI_ = make_unique<Sprite>();

	unique_ptr<Sprite> flag1_ = make_unique<Sprite>();
	unique_ptr<Sprite> flag2_ = make_unique<Sprite>();
	unique_ptr<Sprite> flag3_ = make_unique<Sprite>();

	unique_ptr<Fade> fade_ = make_unique<Fade>();
	unique_ptr<Ranking> ranking_ = make_unique<Ranking>();

	unique_ptr<Object3d> buildingA_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingB_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingC_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingD_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingE_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingF_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingG_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingH_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingI_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingJ_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingK_ = make_unique<Object3d>();
	unique_ptr<Object3d> buildingL_ = make_unique<Object3d>();

	unique_ptr<Object3d> load_ = make_unique<Object3d>();
	unique_ptr<Object3d> load2_ = make_unique<Object3d>();

	unique_ptr<Object3d> skydome_ = make_unique<Object3d>();

	unique_ptr<Object3d> player_ = make_unique<Object3d>();
	unique_ptr<Particles> engineFire_ = make_unique<Particles>();
	EmitterSphere engineFireEmitter_ = {};
	EmitterRange engineFireRange_ = {};

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = false;

	void SceneController();
	void UpdateFade();
	void CameraController();
	void InitObj();
	void CloseSound();

	enum State {
		kPlay,
		kTutorial,
		kQuit,
	};

	State state_ = kPlay;
};
