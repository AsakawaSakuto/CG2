#include"SkyBox.h"

void SkyBox::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	left_->Initialize(dxCommon_, "resources/object3d/skyBox/skyBoxLeft.obj");
	right_->Initialize(dxCommon_, "resources/object3d/skyBox/skyBoxRight.obj");
	top_->Initialize(dxCommon_, "resources/object3d/skyBox/skyBoxTop.obj");
	bottom_->Initialize(dxCommon_, "resources/object3d/skyBox/skyBoxBottom.obj");
}

void SkyBox::Update(Camera* camera) {
	
	uvTranslateLeft_.x += uvSpeed_ * deltaTime_;
	uvTranslateRight_.x -= uvSpeed_ * deltaTime_;
	uvTranslateTop_.y += uvSpeed_ * deltaTime_;
	uvTranslateBottom_.y -= uvSpeed_ * deltaTime_;

	left_->SetUvTranslate(uvTranslateLeft_);
	right_->SetUvTranslate(uvTranslateRight_);
	top_->SetUvTranslate(uvTranslateTop_);
	bottom_->SetUvTranslate(uvTranslateBottom_);

	left_->Update(*camera);
	right_->Update(*camera);
	top_->Update(*camera);
	bottom_->Update(*camera);
}

void SkyBox::Draw() {
	left_->Draw();
	right_->Draw();
	top_->Draw();
	bottom_->Draw();
}

void SkyBox::DrawImGui() {
	left_->DrawImGui("L");
	right_->DrawImGui("R");
}