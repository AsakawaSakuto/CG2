#include "ResultScene.h"

ResultScene::~ResultScene() {
	CleanupResources();
}

void ResultScene::CleanupResources() {

}

void ResultScene::Initialize() {
	ChangeScene(SCENE::RESULT);

	pushA_->Initialize("ui/result/pusha.png", { 640.0f,360.0f }, { 1.0f,1.0f });
}

void ResultScene::Update() {
	if (MyInput::Trigger(Action::CONFIRM)) {
		ChangeScene(SCENE::TITLE);
	}

	pushA_->Update();
}

void ResultScene::Draw() {
	pushA_->Draw();
}

void ResultScene::DrawImGui() {

}

void ResultScene::PostFrameCleanup() {

}