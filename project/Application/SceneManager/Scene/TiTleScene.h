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
	EditType editType_ = EditType::Screen;

	int bgmVolume_ = 5;
	int seVolume_ = 5;
	bool isFullScreen_ = false;
	float titleBgmVolume_ = 0.2f;

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

	unique_ptr<Sprite> fadeBG_;
	GameTimer fadeInTimer_;
	GameTimer fadeOutTimer_;

	Vector3 playerColors[4] = {
	 { 1.000f, 0.447f, 0.133f },
	 { 0.161f, 0.318f, 0.827f },
	 { 0.102f, 0.784f, 0.000f },
	 { 0.239f, 0.855f, 0.922f }
	};
};