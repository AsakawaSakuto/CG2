#include "GameScene.h"

GameScene::~GameScene() {
	CleanupResources();
}

void GameScene::CleanupResources() {

}

void GameScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void GameScene::Initialize() {
	ChangeScene(SCENE::GAME);

	gameCamera_->Initialize(ctx_);
	player_->Initialize(ctx_);

	testPlane_->Initialize(&ctx_->dxCommon, "plane.obj");
	testPlane_->SetScale({ 10.0f,1.0f,10.0f });
}

void GameScene::Update() {
	if (ctx_->gamePad.TriggerButton(GamePad::A)) {
		ChangeScene(SCENE::TITLE);
	}

	player_->Update();

	gameCamera_->SetTarget(player_->GetPosition());
	gameCamera_->Update();

	camera_ = gameCamera_->GetCamera();

	testPlane_->Update();
}

void GameScene::Draw() {
	player_->Draw(camera_);

	testPlane_->Draw(camera_);
}

void GameScene::DrawImGui() {
	DrawSceneName();

	player_->DrawImGui();
}