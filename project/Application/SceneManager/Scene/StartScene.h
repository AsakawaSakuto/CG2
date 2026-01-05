#pragma once
#include"EngineSystem.h"
#include"SceneManager/IScene.h"
#include"GameUI/TitleSceneUI/TitleSceneUI.h"
#include"GameObject/Player/playerStatus.h"
#include"GameObject/Player/WeaponManager/WeaponStatus.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class StartScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void PostFrameCleanup() override;
	~StartScene();

private:
	// リソースクリーンアップメソッド
	void CleanupResources();

private:
	unique_ptr<Sprite> bg_;
	unique_ptr<Sprite> text_;
	GameTimer timer_;
	GameTimer startTimer_;
	GameTimer fadeTimer_;
};