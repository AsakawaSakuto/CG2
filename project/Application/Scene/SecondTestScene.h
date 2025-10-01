#pragma once
#include"Application/EngineSystem.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class SecondTestScene : public IScene {
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

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = false;

	void CameraController();
	void InitObj();
	void CloseSound();

};
