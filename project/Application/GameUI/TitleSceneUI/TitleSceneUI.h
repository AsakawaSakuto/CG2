#pragma once
#include "GameUI/BaseUI.h"
#include "EngineSystem.h"

class TitleSceneUI : public BaseUI {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	~TitleSceneUI();

	bool QuitSelected() { return isQuit_; }
	bool PlaySelected() { return isPlay_; }
private:

	enum class TitleSelectState {
		PLAY,
		QUIT,
	};

	TitleSelectState selectState_ = TitleSelectState::PLAY;

private:

	unique_ptr<Sprite> playUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> quitUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> titleLogoUI_ = make_unique<Sprite>();

	bool isQuit_ = false;
	bool isPlay_ = false;
};