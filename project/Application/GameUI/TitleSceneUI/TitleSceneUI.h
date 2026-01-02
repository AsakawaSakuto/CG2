#pragma once
#include "GameUI/BaseUI.h"
#include "EngineSystem.h"

enum class TitleSelectState {
	PLAY,
	EDIT,
	QUIT,
	SELECT,
};

class TitleSceneUI : public BaseUI {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;

	void SetSelectState(TitleSelectState state) { selectState_ = state; }
private:

	TitleSelectState selectState_ = TitleSelectState::PLAY;

private:

	unique_ptr<Sprite> play_;
	unique_ptr<Sprite> edit_;
	unique_ptr<Sprite> quit_;
	unique_ptr<Sprite> logo_;
	unique_ptr<Sprite> ranking_;

	Vector2 maxScale_ = { 0.8f,0.8f };
	Vector2 minScale_ = { 0.5f,0.5f };
};