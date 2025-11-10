#include "GameScene.h"

GameScene::~GameScene() {
	CleanupResources();
}

void GameScene::CleanupResources() {

}

void GameScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void GameScene::Initialize() {
	ChangeScene(SCENE::GAME);

	player_->Initialize(ctx_);
}

void GameScene::Update() {
	if (ctx_->gamePad.TriggerButton(GamePad::A)) {
		ChangeScene(SCENE::TITLE);
	}

	player_->Update();
}

void GameScene::Draw() {
	player_->Draw(camera_);
}

void GameScene::DrawImGui() {
	DrawSceneName();

	player_->DrawImGui();
}