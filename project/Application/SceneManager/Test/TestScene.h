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
	void DrawImGui() override;
	~TestScene();

private:
	// リソースクリーンアップメソッド
	void CleanupResources();

	void MT4_01_03();
	void MT4_01_04();
	void MT4_01_05();
private:
	// AppContext
	AppContext* ctx_ = nullptr;

	unique_ptr<Particles> testParticle_ = make_unique<Particles>();

	unique_ptr<Model> normalCube_ = make_unique<Model>();
	unique_ptr<Model> animationCube_ = make_unique<Model>();

	unique_ptr<Model> simpleSkin_ = make_unique<Model>();
	unique_ptr<Model> walk_ = make_unique<Model>();
	unique_ptr<Model> sneakWalk_ = make_unique<Model>();

	Transform normalCubeTransform_;
	Transform animationCubeTransform_;
	Transform simpleSkinTransform_;
	Transform walkTransform_;
	Transform sneakWalkTransform_;

	GameTimer testTimer_;

	Camera camera_;
	DebugCamera debugCamera_;
};
