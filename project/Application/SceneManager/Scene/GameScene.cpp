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

	enemyManager_->Initialize(ctx_);

	testPlane_->Initialize(&ctx_->dxCommon, "plane.obj");
	testPlane_->SetScale({ 100.0f,1.0f,100.0f });
	testPlane_->SetTranslate({ 0.0f,-0.5f,0.0f });

	testWall_->Initialize(&ctx_->dxCommon, "wall.obj");
	testWall_->SetTranslate({ 0.0f,-40.0f,0.0f });
}

void GameScene::Update() {
	if (ctx_->gamePad.TriggerButton(GamePad::START)) {
		ChangeScene(SCENE::TITLE);
	}

	player_->Update();

	gameCamera_->SetTarget(player_->GetPosition());
	gameCamera_->Update();

	enemyManager_->Update();

	camera_ = gameCamera_->GetCamera();
	camera_.Update();

	testPlane_->Update();
	testWall_->Update();
}

void GameScene::Draw() {
	testPlane_->Draw(camera_);
	testWall_->Draw(camera_);

	player_->Draw(camera_);

	enemyManager_->Draw(camera_);
}

void GameScene::DrawImGui() {
	DrawSceneName();

	player_->DrawImGui();
}