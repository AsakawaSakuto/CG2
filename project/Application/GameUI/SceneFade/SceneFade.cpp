#include "Application/GameUI/SceneFade/SceneFade.h"

void SceneFade::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	backBlack_->Initialize(dxCommon_, "resources/image/fade.png", { 1280.0f,720.0f });
	loadingText_->Initialize(dxCommon_, "resources/image/loading.png", { 1280.0f,720.0f });

	backBlack_->SetPosition({ 640.0f,360.0f });
	loadingText_->SetPosition({ 640.0f,360.0f });

	backBlack_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	loadingText_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
}

void SceneFade::Update() {
	fadeInTimer_.Update();
	fadeOutTimer_.Update();

	// フェードイン処理（透明から不透明へ）
	if (fadeInTimer_.IsActive()) {
		fadeAlpha_ = fadeInTimer_.GetProgress();
	}
	// フェードアウト処理（不透明から透明へ）
	else if (fadeOutTimer_.IsActive()) {
		fadeAlpha_ = fadeOutTimer_.GetProgress();
	}

	backBlack_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });
	loadingText_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });

	backBlack_->Update();
	loadingText_->Update();
}

void SceneFade::Draw() {
	backBlack_->Draw();
	loadingText_->Draw();
}