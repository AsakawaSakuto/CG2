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

private:

};