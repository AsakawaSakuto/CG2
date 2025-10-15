#pragma once
#include"Application/EngineSystem.h"
#include"Application/SceneManager/IScene.h"
#include "Application/GameObject/Text3D/Text3D.h"
#include <array>

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
	unique_ptr<Particles> particle2_ = make_unique<Particles>();

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ
	void CameraController();     

	// 3Dテキスト
	std::array<std::unique_ptr<Text3D>, 5> text3D_;
};
