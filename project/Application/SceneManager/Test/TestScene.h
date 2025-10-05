#pragma once
#include"Application/EngineSystem.h"
#include"Application/SceneManager/IScene.h"

#include"Application/GameObject/Player/Player.h"

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
	// AppContext
	AppContext* ctx_ = nullptr;

	// InputSystem
	GamePad* gamePad_ = nullptr;
	Input* input_ = nullptr;

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = false;                                       // デバッグカメラ使用フラグ
	void CameraController();

	// Objects
	unique_ptr<Model> model_ = make_unique<Model>();
	unique_ptr<Sprite> sprite_ = make_unique<Sprite>();
	unique_ptr<Player> player_ = make_unique<Player>();

	// UtilSystem
	GameTimer gameTimer_;

	//
	Vector3 startPos_ = { 0.0f,0.0f,0.0f };
	Vector3 endPos_ = { 10.0f,0.0f,0.0f };
};
