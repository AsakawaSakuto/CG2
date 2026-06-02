#pragma once
#include"EngineSystem.h"
#include"SceneManager/IScene.h"
#include"Utility/Collision/Type/OBB.h"
#include"Utility/Collision/Type/OvalSphere.h"
#include"2d/Gauge/Gauge.h"
#include"3d/SkyBox/SkyBox.h"
#include"3d/Effect/Effect.h"
#include"3d/Effect/EffectFactory.h"
#include"3d/Ring/RingRenderer.h"

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
	DebugCamera debugCamera_;
	SkyBox skyBox_;
	Model model_;

	// --- Effect ---
	std::unique_ptr<Effect> hitEffect_;
	std::unique_ptr<Effect> slashEffect_;
	Vector3 effectSpawnPos_ = { 0.0f, 0.0f, 0.0f };

	// --- Ring ---
	RingRenderer ring_;
	float uvScrollSpeed_ = 0.5f;
	float uvScrollTimer_ = 0.0f;
};
