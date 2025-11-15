#include "TitleSceneUI.h"

TitleSceneUI::~TitleSceneUI() {
}

void TitleSceneUI::Initialize(AppContext* ctx) {
	ctx_ = ctx;

	playUI_->Initialize(&ctx_->dxCommon, "UI/title/playUI", { 640.0f,440.0f }, { 0.5f,0.5f });
	quitUI_->Initialize(&ctx_->dxCommon, "UI/title/quitUI", { 640.0f,600.0f }, { 0.5f,0.5f });
	titleLogoUI_->Initialize(&ctx_->dxCommon, "UI/title/titlelogo", { 640.0f,200.0f });
}

void TitleSceneUI::Update() {

	switch (selectState_)
	{
	case TitleSceneUI::TitleSelectState::PLAY:

		if (ctx_->gamePad.TriggerButton(GamePad::DOWN_BOTTON)) {
			selectState_ = TitleSelectState::QUIT;
		}

		if (ctx_->gamePad.TriggerButton(GamePad::A) || ctx_->input.TriggerKey(DIK_SPACE)) {
			isPlay_ = true;
		}

		playUI_->SetScale({ 0.55f,0.55f });
		quitUI_->SetScale({ 0.5f,0.5f });

		break;
	case TitleSceneUI::TitleSelectState::QUIT:

		if (ctx_->gamePad.TriggerButton(GamePad::UP_BOTTON)) {
			selectState_ = TitleSelectState::PLAY;
		}

		if (ctx_->gamePad.TriggerButton(GamePad::A)) {
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