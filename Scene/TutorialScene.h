#pragma once
#include"IScene.h"
#include"AppContext.h"
#include"DebugCamera.h"

#include "Fade.h"

class TutorialScene : public IScene
{
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
private:
	AppContext* ctx_ = nullptr;

	GamePad* gamePad_ = nullptr;

	unique_ptr<Fade> fade_ = make_unique<Fade>();

	// Cameras
	unique_ptr<Camera> camera_ = make_unique<Camera>();
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>();
	Camera* useCamera_ = nullptr;
	bool isDebugCamera_ = false;

	void CameraController();
};