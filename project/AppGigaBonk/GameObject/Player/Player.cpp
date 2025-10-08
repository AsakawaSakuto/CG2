#include "player.h"

void Player::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/gigabonk/model/player/player.obj");

	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,0.0f,0.0f };
	CollitionSphere_.center = transform_.translate;
	CollitionSphere_.radius = 1.0f;
}

void Player::Update() {
	
	if (input_->PushKey(DIK_A)) {
		transform_.translate.x -= 5.0f * deltaTime_;	
	}

	if (input_->PushKey(DIK_D)) {
		transform_.translate.x += 5.0f * deltaTime_;
	}

	// 当たり判定用の球の中心を更新
	CollitionSphere_.center = transform_.translate;

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();
}

void Player::Draw(Camera useCamera) {
	model_->Draw(useCamera);
}