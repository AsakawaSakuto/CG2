#pragma once
#include "Application/EngineSystem.h"
#include "Application/SceneManager/IScene.h"
#include <array>

#include "Application/GameObject/ResultObject/Score.h"

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::unique_ptr;

class ResultScene : public IScene {
private:

public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	~ResultScene();

private:
	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = false;                                       // デバッグカメラ使用フラグ
	void CameraController();

	float lastScore_ = 0.0f; // 前のシーンから受け取るスコア

	unique_ptr<Score> score_ = make_unique<Score>();
};
