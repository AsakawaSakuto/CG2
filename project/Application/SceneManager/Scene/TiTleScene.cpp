#include "TiTleScene.h"

TitleScene::~TitleScene() {
	CleanupResources();
}

void TitleScene::CleanupResources() {

}

void TitleScene::Initialize() {
	ChangeScene(SCENE::TITLE);

	titleUI_->Initialize();
}

void TitleScene::Update() {
	if (titleUI_->PlaySelected()) {
		ChangeScene(SCENE::GAME);
	} else if (titleUI_->QuitSelected()) {
		Quit();
	}

	titleUI_->Update();
}

void TitleScene::Draw() {

	titleUI_->Draw();
	
}

void TitleScene::DrawImGui() {
	
	titleUI_->DrawImGui();
}

void TitleScene::PostFrameCleanup() {

}