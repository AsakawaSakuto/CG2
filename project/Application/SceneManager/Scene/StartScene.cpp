#include "StartScene.h"
#include "Core/ServiceLocator/ServiceLocator.h"

StartScene::~StartScene() {
	CleanupResources();
}

void StartScene::CleanupResources() {

}

void StartScene::Initialize() {
	ChangeScene(SCENE::START);

	bg_ = std::make_unique<Sprite>();
	bg_->Initialize("startSprite.png", { 0.0f,0.0f }, { 1.0f,1.0f });

	text_ = std::make_unique<Sprite>();
	text_->Initialize("text.png", { 660.0f,160.0f }, { 0.35f,0.4f });
	text_->SetAnchorPoint(AnchorPoint::Center);

	startTimer_.Start(1.0f, false);
	MyAudio::PlaySE(SE_List::StartUp);
}

void StartScene::Update() {
	timer_.Update();
	fadeTimer_.Update();
	startTimer_.Update();

	if (startTimer_.IsFinished()) {
		startTimer_.Reset();
		timer_.Start(3.0f, false);
	}

	if (startTimer_.IsActive()) {
		bg_->SetColor({ 1.0f,1.0f,1.0f, startTimer_.GetProgress() });
		text_->SetColor({ 1.0f,1.0f,1.0f, startTimer_.GetProgress() });
	}

	if (timer_.IsFinished()) {
		timer_.Reset();
		fadeTimer_.Start(1.0f, false);
	}

	if (fadeTimer_.IsActive()) {
		bg_->SetColor({ 1.0f,1.0f,1.0f, fadeTimer_.GetReverseProgress() });
		text_->SetColor({ 1.0f,1.0f,1.0f, fadeTimer_.GetReverseProgress() });
	}

	if (fadeTimer_.IsFinished()) {
		MyAudio::StopSE(SE_List::StartUp);
		ChangeScene(SCENE::TITLE);
	}

	bg_->Update();
	text_->Update();
}

void StartScene::Draw() {
	bg_->Draw();
	text_->Draw();
}

void StartScene::DrawImGui() {
	//text_->DrawImGui("StartSceneText");
}

void StartScene::PostFrameCleanup() {

}