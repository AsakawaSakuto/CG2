#include "TitleSceneUI.h"

void TitleSceneUI::Initialize() {

	playUI_->Initialize("UI/title/playUI.png", { 640.0f,440.0f }, { 0.5f,0.5f });
	quitUI_->Initialize("UI/title/quitUI.png", { 640.0f,600.0f }, { 0.5f,0.5f });
	titleLogoUI_->Initialize("UI/title/titlelogo.png", { 640.0f,200.0f });
}

void TitleSceneUI::Update() {

	switch (selectState_)
	{
	case TitleSceneUI::TitleSelectState::PLAY:

		if (MyInput::Trigger(Action::CELECT_DOWN)) {
			selectState_ = TitleSelectState::QUIT;
		}

		if (MyInput::Trigger(Action::CONFIRM)) {
			isPlay_ = true;
		}

		playUI_->SetScale({ 0.55f,0.55f });
		quitUI_->SetScale({ 0.5f,0.5f });

		break;
	case TitleSceneUI::TitleSelectState::QUIT:

		if (MyInput::Trigger(Action::CELECT_UP)) {
			selectState_ = TitleSelectState::PLAY;
		}

		if (MyInput::Trigger(Action::CONFIRM)) {
			isQuit_ = true;
		}

		playUI_->SetScale({ 0.5f,0.5f });
		quitUI_->SetScale({ 0.55f,0.55f });

		break;
	default:
		break;
	}

	playUI_->Update();
	quitUI_->Update();
	titleLogoUI_->Update();
}

void TitleSceneUI::Draw() {
	playUI_->Draw();
	quitUI_->Draw();
	titleLogoUI_->Draw();
}

void TitleSceneUI::DrawImGui() {
	playUI_->DrawImGui("PlayUI");
	quitUI_->DrawImGui("QuitUI");
	titleLogoUI_->DrawImGui("TitleLogoUI");
}