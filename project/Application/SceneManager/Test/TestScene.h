#pragma once
#include"Application/EngineSystem.h"
#include"Application/SceneManager/IScene.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class TestScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	~TestScene();

private:
	// リソースクリーンアップメソッド
	void CleanupResources();

private:
	// AppContext
	AppContext* ctx_ = nullptr;

	unique_ptr<Particles> testParticle_ = make_unique<Particles>();
	Camera camera_;
};
