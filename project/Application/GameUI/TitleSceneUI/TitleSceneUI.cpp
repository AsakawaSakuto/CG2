#include "TitleSceneUI.h"

void TitleSceneUI::Initialize() {
	play_ = std::make_unique<Sprite>();
	play_->Initialize("UI/Title/play.png");
	play_->LoadFromJson("PlayUI");

	quit_ = std::make_unique<Sprite>();
	quit_->Initialize("UI/Title/quit.png");
	quit_->LoadFromJson("QuitUI");

	edit_ = std::make_unique<Sprite>();
	edit_->Initialize("UI/Title/edit.png");
	edit_->LoadFromJson("EditUI");

	logo_ = std::make_unique<Sprite>();
	logo_->Initialize("UI/Title/gigabonk.png");

	ranking_ = std::make_unique<Sprite>();
	ranking_->Initialize("UI/Title/ranking.png");
}

void TitleSceneUI::Update() {

	switch (selectState_) {
	case TitleSelectState::PLAY:

		play_->SetScale(maxScale_);
		edit_->SetScale(minScale_);
		quit_->SetScale(minScale_);

		break;
	case TitleSelectState::EDIT:

		play_->SetScale(minScale_);
		edit_->SetScale(maxScale_);
		quit_->SetScale(minScale_);

		break;
	case TitleSelectState::QUIT:

		play_->SetScale(minScale_);
		edit_->SetScale(minScale_);
		quit_->SetScale(maxScale_);

		break;

	case  TitleSelectState::SELECT:

		break;
	default:
		break;
	}

	play_->   Update();
	edit_->   Update();
	quit_->   Update();
	logo_->   Update();
	ranking_->Update();
}

void TitleSceneUI::Draw() {
	if (selectState_ != TitleSelectState::SELECT) {
		play_->Draw();
		edit_->Draw();
		quit_->Draw();
		//logo_->Draw();
		//ranking_->Draw();
	}
}

void TitleSceneUI::DrawImGui() {
	//play_->DrawImGui("TitlePlayUI");
	//edit_->DrawImGui("TitleEditUI");
	//quit_->DrawImGui("TitleQuitUI");
	//logo_->DrawImGui("TitleLogoUI");
	//ranking_->DrawImGui("TitleRankingUI");
}