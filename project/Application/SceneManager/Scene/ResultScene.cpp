#include "ResultScene.h"

ResultScene::~ResultScene() {
	CleanupResources();
}

void ResultScene::CleanupResources() {

}

void ResultScene::Initialize() {
	ChangeScene(SCENE::RESULT);

	pushA_->Initialize("loading.png", { 0.0f,0.0f }, { 1.0f,1.0f });
}

void ResultScene::Update() {
	pushA_->Update();
	ChangeScene(SCENE::GAME);
}

void ResultScene::Draw() {
	pushA_->Draw();
}

void ResultScene::DrawImGui() {

}

void ResultScene::PostFrameCleanup() {

}