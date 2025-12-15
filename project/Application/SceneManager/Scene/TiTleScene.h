#pragma once
#include"EngineSystem.h"
#include"SceneManager/IScene.h"
#include"GameUI/TitleSceneUI/TitleSceneUI.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class TitleScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void PostFrameCleanup() override;
	~TitleScene();

private:
	// リソースクリーンアップメソッド
	void CleanupResources();

private:
	// AppContext
	AppContext* ctx_ = nullptr;

	Camera camera_;
	DebugCamera debugCamera_;

	unique_ptr<TitleSceneUI> titleUI_ = make_unique<TitleSceneUI>();
};