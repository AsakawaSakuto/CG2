#include "ResultScene.h"

ResultScene::~ResultScene() {
	CleanupResources();
}

void ResultScene::CleanupResources() {

}

void ResultScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void ResultScene::Initialize() {
	ChangeScene(SCENE::RESULT);

	pushA_->Initialize(&ctx_->dxCommon, "ui/result/pusha.png", { 640.0f,360.0f }, { 1.0f,1.0f });
}

void ResultScene::Update() {
	if (ctx_->gamePad.TriggerButton(GamePad::A)) {
		ChangeScene(SCENE::TITLE);
	}

	pushA_->Update();
}

void ResultScene::Draw() {
	pushA_->Draw();
}

void ResultScene::DrawImGui() {

}