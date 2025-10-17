#pragma once
#include "Application/EngineSystem.h"
#include "Application/GameObject/Text3D/Text3D.h"
#include "Application/SceneManager/IScene.h"
#include <array>

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::unique_ptr;

class TitleScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	~TitleScene();

private:

	// リソースクリーンアップメソッド
	void CleanupResources();

private:
	unique_ptr<Particles> particle_ = make_unique<Particles>();
	unique_ptr<Particles> particle2_ = make_unique<Particles>();

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ

	void CameraController();     

	unique_ptr<Text3D> titleLogo_ = make_unique<Text3D>();

	unique_ptr<Sprite> playUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> optionUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> cursolUI_ = make_unique<Sprite>();
};
