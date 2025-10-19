#pragma once
#include "Application/EngineSystem.h"
#include "Application/GameObject/Text3D/Text3D.h"
#include "Application/GameObject/TitleObject/TitleObject.h"
#include "Application/SceneManager/IScene.h"
#include <array>

using Microsoft::WRL::ComPtr;
using std::make_unique;
using std::unique_ptr;

enum Menu {
	PLAY,
	OPTION,
};

enum OptionMenu {
	NONE,
	FULLSCREEN,
	SE,
	BGM,
	BACK,
};

enum SoundVolume {
	DAI,
	TYU,
	SYOU,
};

class TitleScene : public IScene {
public:
	void SetAppContext(AppContext* ctx) override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	~TitleScene();

private:

	// リソースクリーンアップメソッド
	void CleanupResources();

	void TitleLogoUpdate();
	void SelectUIUpdate();
	void OptionUIUpdate();

	void SpriteUpdate();
	void AudioUpdate();
private:

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ

	void CameraController();     

	unique_ptr<Text3D> titleLogo_ = make_unique<Text3D>();
	unique_ptr<TitleObject> titleObject_ = make_unique<TitleObject>();

	unique_ptr<Model> cloud_ = make_unique<Model>();

	unique_ptr<Sprite> playUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> optionUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> cursolUI_ = make_unique<Sprite>();

	unique_ptr<Sprite> optionBG_ = make_unique<Sprite>();
	unique_ptr<Sprite> optionCursolUI_ = make_unique<Sprite>();

	unique_ptr<Sprite> fullScreenUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> onUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> offUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> seUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> bgmUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> daiUI_ = make_unique<Sprite>(); 
	unique_ptr<Sprite> dai2UI_ = make_unique<Sprite>();
	unique_ptr<Sprite> tyuUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> tyu2UI_ = make_unique<Sprite>();
	unique_ptr<Sprite> syouUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> syou2UI_ = make_unique<Sprite>();
	unique_ptr<Sprite> backUI_ = make_unique<Sprite>();

	unique_ptr<AudioX> startGameSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> moveCursolSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> decideSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> titleSceneBGM_ = make_unique<AudioX>();

	float uiAlpha_ = 0.0f;

	GameTimer titleTimer_;
	GameTimer cursolTimer_;
	GameTimer optionTimer_;
	GameTimer optionCursolTimer_;

	Vector2 optionBGScale_;
	Vector2 optionCursolUIScale_;
	Vector2 normalUIScale_;

	float optionCursolStart_;
	float optionCursolEnd_;

	bool optionOpen_ = false;

	float daiVolumeSE_ = 1.0f;
	float tyuVolumeSE_ = 0.5f;
	float syouVolumeSE_ = 0.2f;

	float daiVolumeBGM_ = 0.7f;
	float tyuVolumeBGM_ = 0.3f;
	float syouVolumeBGM_ = 0.1f;

	Menu selectMenu_ = Menu::PLAY;
	OptionMenu selectOptionMenu_ = OptionMenu::NONE;
	SoundVolume seVolume_ = SoundVolume::TYU;
	SoundVolume bgmVolume_ = SoundVolume::TYU;
};
