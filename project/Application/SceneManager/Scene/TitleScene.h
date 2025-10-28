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
	QUIT,
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

	void InitSptite();

	void CloudUpdate();
	void TitleLogoUpdate();
	void SelectUIUpdate();
	void OptionUIUpdate();

	void SpriteUpdate();
	void AudioUpdate();
private:

	enum class MaskType {
		RAMA = 0,
		KUMA = 1,
		AME = 2,
	};

	MaskType maskType_ = MaskType::AME;

	enum class BG_Type {
		SKY,
		GRY,
		NIGHT,
		ORANGE,  // 淡い夕焼けオレンジ色を追加
	};

	BG_Type bgType_ = BG_Type::SKY;

	GameTimer bgChangeTimer_;
	GameTimer bgFadeTimer_;
	Vector3 bgColorStart_;
	Vector3 bgColorEnd_;

	void ChangeBG();

	// Camera
	Camera* useCamera_ = nullptr;                                      // 使用するカメラ
	unique_ptr<Camera> normalCamera_ = make_unique<Camera>();          // ノーマルカメラ
	unique_ptr<DebugCamera> debugCamera_ = make_unique<DebugCamera>(); // デバッグカメラ
	bool useDebugCamera_ = true;                                       // デバッグカメラ使用フラグ

	bool pushStart_ = false;

	void CameraController();     

	unique_ptr<Text3D> titleLogo_ = make_unique<Text3D>();
	unique_ptr<TitleObject> titleObject_ = make_unique<TitleObject>();

	std::array<unique_ptr<Model>,10> cloud_;
	std::array<Transform, 10> cloudTramsform_;
	std::array<bool, 10>cloudIsActive_ = { false };
	GameTimer cloudTimer_;

	std::array<unique_ptr<Model>, 10> cloudLine_;
	std::array<Transform, 10> cloudLineTramsform_;
	std::array<bool, 10>cloudLineIsActive_ = { false };
	GameTimer cloudLineTimer_;

	unique_ptr<Sprite> playUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> optionUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> quitUI_ = make_unique<Sprite>();
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
	unique_ptr<Sprite> OptionBearUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> uiBoxUI_ = make_unique<Sprite>();

	unique_ptr<Sprite> parenthesesUI1_ = make_unique<Sprite>();
	unique_ptr<Sprite> parenthesesUI2_ = make_unique<Sprite>();
	unique_ptr<Sprite> parenthesesUI3_ = make_unique<Sprite>();
	unique_ptr<Sprite> parenthesesUI4_ = make_unique<Sprite>();
	unique_ptr<Sprite> parenthesesUI5_ = make_unique<Sprite>();
	unique_ptr<Sprite> parenthesesUI6_ = make_unique<Sprite>();

	unique_ptr<AudioX> startGameSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> moveCursolSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> decideSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> titleSceneBGM_ = make_unique<AudioX>();

	unique_ptr<Particles> titleParticle_ = make_unique<Particles>();

	unique_ptr<Particles> testParticle_ = make_unique<Particles>();

	float uiAlpha_ = 0.0f;

	GameTimer titleTimer_;
	GameTimer cursolTimer_;
	GameTimer optionTimer_;
	GameTimer optionCursolTimer_;
	GameTimer fullScreenTimer_;

	float startCursolY_;
	float endCursolY_;
	Vector2 optionBGScale_;
	Vector2 optionCursolUIScale_;
	Vector2 normalUIScale_;
	Vector2 parenthesesUIScale_;

	float optionCursolStart_;
	float optionCursolEnd_;

	bool optionOpen_ = false;

	float daiVolumeSE_ = 0.2f;
	float tyuVolumeSE_ = 0.1f;
	float syouVolumeSE_ = 0.05f;

	float daiVolumeBGM_ = 0.3f;
	float tyuVolumeBGM_ = 0.1f;
	float syouVolumeBGM_ = 0.05f;

	Menu selectMenu_ = Menu::PLAY;
	OptionMenu selectOptionMenu_ = OptionMenu::NONE;
	SoundVolume seVolume_ = SoundVolume::TYU;
	SoundVolume bgmVolume_ = SoundVolume::TYU;

	Random random_;

	// mask
	unique_ptr<Sprite> mask_ = make_unique<Sprite>();
	unique_ptr<Sprite> maskBox_ = make_unique<Sprite>();
	unique_ptr<Sprite> loadingUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> loadingPlayer_ = make_unique<Sprite>();
	Vector2 maskStartPos_;
	Vector2 maskEndPos_;
	Vector2 maskStartScale_;
	Vector2 maskEndScale_;
	GameTimer maskTimer_;

	bool titleQuit_ = false;
};
