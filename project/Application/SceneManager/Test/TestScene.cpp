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

	cube_->Initialize(&ctx_->dxCommon, "cube.obj");
	cubeTransform_.scale = { 1.0f,1.0f,1.0f };

	testTimer_.Start(1.0f, true);
}

void TestScene::Update() {

	testParticle_->Update();

	//cubeTransform_.translate = Easing::LerpVector3(
	//	{ 0.0f,0.0f,0.0f }, { 10.0f,5.0f,3.0f }, testTimer_.GetProgress(),
	//	Easing::Type::EaseOutInBack);

	cubeTransform_.translate = Easing::LerpVector3_GAB(
		{ 0.0f,0.0f,0.0f }, { 10.0f,5.0f,3.0f }, testTimer_.GetProgress(), 
		Easing::Type::Linear, Easing::Type::EaseOutBounce);

	cube_->SetTransform(cubeTransform_);
	cube_->Update();

	testTimer_.Update();

    camera_ = debugCamera_;
	camera_.Update();
	debugCamera_.Update();
}

void TestScene::Draw() {
	cube_->Draw(camera_);
	testParticle_->Draw(camera_);
}

void TestScene::DrawImGui() {

    DrawSceneName();

    testParticle_->DrawImGui("TestParticle");

	cube_->DrawImGui("TestCube");
}
