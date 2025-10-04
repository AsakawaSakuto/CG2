#pragma once
#include"Application/EngineSystem.h"
#include"Application/SceneManager/IScene.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class ResultScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	// AppContext
	AppContext* ctx_ = nullptr;

	// InputSystem
	GamePad* gamePad_ = nullptr;
	Input* input_ = nullptr;

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ
	void CameraController();
};
