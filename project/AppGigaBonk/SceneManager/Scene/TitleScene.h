#pragma once
#include"AppGigaBonk/EngineSystem.h"
#include"AppGigaBonk/SceneManager/IScene.h"

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

	unique_ptr<Particles> particle_ = make_unique<Particles>();

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ
	void CameraController();                                           
};
