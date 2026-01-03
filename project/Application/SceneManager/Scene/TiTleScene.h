#pragma once
#include"EngineSystem.h"
#include"SceneManager/IScene.h"
#include"GameUI/TitleSceneUI/TitleSceneUI.h"
#include"GameObject/Player/playerStatus.h"
#include"GameObject/Player/WeaponManager/WeaponStatus.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;

class TitleScene : public IScene {
public:
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

	Camera camera_;
	DebugCamera debugCamera_;

	TitleSelectState selectState_ = TitleSelectState::Play;

	PlayerName playerName_ = PlayerName::PowerMan;
	WeaponName weaponName_ = WeaponName::FireBall;

	unique_ptr<TitleSceneUI> titleUI_;
	unique_ptr<SkiningModel> player_;
	Transform playerTransform_;

	unique_ptr<Model> block_;
	Transform blockTransform_;
	unique_ptr<Model> block2_;
	Transform block2Transform_;
	unique_ptr<Model> block3_;
	Transform block3Transform_;
	unique_ptr<Model> slope_;
	Transform slopeTransform_;
	unique_ptr<Model> tree_;
	Transform treeTransform_;
};