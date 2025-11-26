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
	void DrawImGui() override;
	void PostFrameCleanup() override;
	~ResultScene();

private:
	// リソースクリーンアップメソッド
	void CleanupResources();

private:
	unique_ptr<Sprite> pushA_ = make_unique<Sprite>();

	// AppContext
	AppContext* ctx_ = nullptr;

	Camera camera_;
	DebugCamera debugCamera_;
};