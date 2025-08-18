#include"Fade.h"

void Fade::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	fade_->Initialize(dxCommon_, "resources/image/fade.png", { 1280.0f,720.0f });
	fade_->SetPosition({ 640.0f,360.0f });
	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });

	loadingUI_->Initialize(dxCommon_, "resources/image/UI/loading.png", { 1280.0f,720.0f });
	loadingUI_->SetPosition({ 640.0f,360.0f });
	loadingUI_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });
}

void Fade::Update() {
	if (isFade_) {
		fadeAlpha_ += fadeInSpeed_ * deltaTime_;
	} else {
		fadeAlpha_ -= fadeOutSpeed_ * deltaTime_;
	}
	fadeAlpha_ = std::clamp(fadeAlpha_, 0.0f, 1.0f);

	loadingUI_->SetColor({ 1.0f,1.0f,1.0f,fadeAlpha_ });
	loadingUI_->Update();

	fade_->SetColor({ 0.0f,0.0f,0.0f,fadeAlpha_ });
	fade_->Update();
}

void Fade::Draw() {
	fade_->Draw();
	loadingUI_->Draw();
}