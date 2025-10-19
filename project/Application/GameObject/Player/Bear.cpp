#include "Bear.h"

void Bear::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	modelBearHead_ = std::make_unique<Model>();
	modelBearHead_->Initialize(dxCommon_, "player/Head/Head.obj");
	modelBearHead_->SetTexture("resources/model/player/Head/Player.png");

	transform_.scale = {1.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};
	collisionSphere_.center = transform_.translate;
	collisionSphere_.radius = 1.0f;
}

void Bear::Update() {

	// 当たり判定用の球の中心を更新
	collisionSphere_.center = transform_.translate;

	// モデルに座標情報を反映
	modelBearHead_->SetTransform(transform_);
	modelBearHead_->Update();
}

void Bear::Draw(Camera useCamera) { modelBearHead_->Draw(useCamera); }