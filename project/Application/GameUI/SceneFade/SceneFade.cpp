#include "Application/GameUI/SceneFade/SceneFade.h"

void SceneFade::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	backBlack_->Initialize(dxCommon_, "resources/image/fade.png");
	loadingText_->Initialize(dxCommon_, "resources/image/loading.png");

	backBlack_->SetPosition({ 640.0f,360.0f });
	loadingText_->SetPosition({ 640.0f,360.0f });

	backBlack_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	loadingText_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
}

void SceneFade::Update() {
	fadeInTimer_.Update();
	fadeOutTimer_.Update();

	// --- フェードイン（透明→黒） ---
	if (fadeInTimer_.IsActive()) {
		fadeAlpha_ = fadeInTimer_.GetProgress();  // 0 → 1
	}

	// --- フェードアウト（黒→透明） ---
	else if (fadeOutTimer_.IsActive()) {
		fadeAlpha_ = 1.0f - fadeOutTimer_.GetProgress();  // 1 → 0
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