#include"Boss.h"

void Boss::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	body_->Initialize(dxCommon_, "resources/object3d/boss/body.obj");
	body_->SetTranslate({ 0.0f,0.0f,50.0f });
	left_->Initialize(dxCommon_, "resources/object3d/boss/left.obj");
	left_->SetTranslate({ -10.0f,0.0f,50.0f });
	right_->Initialize(dxCommon_, "resources/object3d/boss/right.obj");
	right_->SetTranslate({ 10.0f,0.0f,50.0f });
}

void Boss::Update(Camera* camera) {
	body_->Update(*camera);
	left_->Update(*camera);
	right_->Update(*camera);
}

void Boss::Draw() {
	body_->Draw();
	left_->Draw();
	right_->Draw();
}

void Boss::DrawImGui() {

}