#pragma once
#include"AppGigaBonk/EngineSystem.h"
#include"AppGigaBonk/SceneManager/IScene.h"
#include"AppGigaBonk/GameObject/Player/Player.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class GameScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	int goSceneNum_ = 0; // 0ならタイトル、1ならリザルト

	unique_ptr<Model> ground_ = make_unique<Model>();
	unique_ptr<Player> player_ = make_unique<Player>();

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ
	void CameraController();
};
