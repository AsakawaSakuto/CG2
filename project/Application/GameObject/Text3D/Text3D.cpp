#include "Text3D.h"
#include <algorithm>

void Text3D::Initialize(DirectXCommon* dxCommon) { 
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "cube.obj");

	transform_.translate = {0.0f, 0.0f, 0.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.scale = {1.0f, 1.0f, 1.0f};
}

void Text3D::Update() {
	model_->SetTransform(transform_);
	model_->Update(); 
}

void Text3D::Draw(Camera useCamera) { model_->Draw(useCamera); }

void Text3D::MoveTextAnimation(const float startPosX, const float endPosX, const float startRotY, const float endRotY) {
	// 座標Xの補間
	timer_ += deltaTime_;
	float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);

	// イージング
	float easeT = 1.0f - powf(1.0f - t, 3.0f);

	transform_.translate.x = Lerp(startPosX, endPosX, easeT);
	transform_.rotate.y = Lerp(startRotY, endRotY, easeT);
}
