#include"temp.h"

void Temp::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "modelPath");

	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,0.0f,0.0f };
	CollitionSphere_.center = transform_.translate;
	CollitionSphere_.radius = 1.0f;
}

void Temp::Update() {

	// 当たり判定用の球の中心を更新
	CollitionSphere_.center = transform_.translate;

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();
}

void Temp::Draw(Camera useCamera) {
	model_->Draw(useCamera);
}