#pragma once
#include"Application/EngineSystem.h"
#include"Application/SceneManager/IScene.h"
#include"Engine/System/Utility/Math/OBB.h"

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
	void PostFrameCleanup() override;
	~TestScene();
private:

	// リソースクリーンアップメソッド
	void CleanupResources();

	void MT4_01_01();
	void MT4_01_02();
	void MT4_01_03();
	void MT4_01_04();
	void MT4_01_05();
private:
	// AppContext
	AppContext* ctx_ = nullptr;

	unique_ptr<Sprite> testSprite_ = make_unique<Sprite>();

	unique_ptr<Particles> testParticle_ = make_unique<Particles>();

	unique_ptr<Model> cube_ = make_unique<Model>();
	unique_ptr<Model> spinCube_ = make_unique<Model>();
	unique_ptr<Model> simpleSkin_ = make_unique<Model>();
	unique_ptr<Model> walk_ = make_unique<Model>();
	unique_ptr<Model> sneakWalk_ = make_unique<Model>();

	unique_ptr<Line> testLine_ = make_unique<Line>();

	Transform cubeTransform_;
	Transform spinCubeTransform_;
	Transform simpleSkinTransform_;
	Transform walkTransform_;
	Transform sneakWalkTransform_;

	Animation walkAnimation_;
	Animation sneakWalkAnimation_;

	GameTimer testTimer_;

	Camera camera_;
	DebugCamera debugCamera_;

	Sphere testSphere_;
	AABB testAABB1_;
	AABB testAABB2_;
	OBB testOBB1_;
	OBB testOBB2_;
};
