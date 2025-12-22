#pragma once
#include"EngineSystem.h"
#include"SceneManager/IScene.h"
#include"Utility/Collision/Type/OBB.h"
#include"Utility/Collision/Type/OvalSphere.h"
#include"2d/Gauge/Gauge.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class TestScene : public IScene {
public:
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

	unique_ptr<Sprite> testSprite_ = make_unique<Sprite>();

	unique_ptr<Particles> testParticle_ = make_unique<Particles>();

	unique_ptr<Model> cube_ = make_unique<Model>();
	unique_ptr<Model> spinCube_ = make_unique<Model>();
	unique_ptr<Model> simpleSkin_ = make_unique<Model>();
	unique_ptr<Model> walk_ = make_unique<Model>();
	unique_ptr<Model> sneakWalk_ = make_unique<Model>();

	unique_ptr<Gauge> testGauge_ = make_unique<Gauge>();
	float currentGaugeValue_ = 0.0f;
	float maxGaugeValue_ = 100.0f;

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

	AABB testAABB_;
	OBB testOBB_;
	Sphere testSphere_;
	OvalSphere testOvalSphere_;
	Plane testPlane_;

	BitmapFont bitmapFont_;
	int setValue_ = 0;
};