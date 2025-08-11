#include"Player.h"

void Player::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
	model_->Initialize(dxCommon_, "resources/object3d/player/player.obj");
}

void Player::Update(Camera* camera) {
	model_->Update(*camera);
}

void Player::Draw() {
	model_->Draw();
}