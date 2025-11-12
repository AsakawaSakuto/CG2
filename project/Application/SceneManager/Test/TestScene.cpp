#include "TestScene.h"

TestScene::~TestScene() {
    CleanupResources();
}

void TestScene::CleanupResources() {

}

void TestScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void TestScene::Initialize() {
    CleanupResources();
	testParticle_->Initialize(&ctx_->dxCommon);
	camera_.SetPosition({ 0.0f, 0.0f, -10.0f });
	debugCamera_.SetInput(&ctx_->input);
	debugCamera_.SetPosition({ 0.0f, 0.0f, -10.0f });
}

void TestScene::Update() {
	testParticle_->Update();

    camera_ = debugCamera_;
	camera_.Update();
	debugCamera_.Update();
}

void TestScene::Draw() {

	testParticle_->Draw(camera_);
}

void TestScene::DrawImGui() {

    DrawSceneName();

    testParticle_->DrawImGui("TestParticle");
}