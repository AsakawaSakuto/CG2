#include "player.h"

void Player::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	model_->Initialize(&ctx_->dxCommon, "player/player.obj");
	transform_.scale = { 1.0f,1.0f,1.0f };
}

void Player::Update() {

	if (ctx_->gamePad.PushButton(GamePad::UP_BOTTON)) {
		transform_.translate.z += 2.0f * deltaTime_;
	}
	if (ctx_->gamePad.PushButton(GamePad::DOWN_BOTTON)) {
		transform_.translate.z -= 2.0f * deltaTime_;
	}
	if (ctx_->gamePad.PushButton(GamePad::LEFT_BOTTON)) {
		transform_.translate.x -= 2.0f * deltaTime_;
	}
	if (ctx_->gamePad.PushButton(GamePad::RIGHT_BOTTON)) {
		transform_.translate.x += 2.0f * deltaTime_;
	}

	model_->SetTransform(transform_);
	model_->Update();
}

void Player::Draw(Camera camera) {
	model_->Draw(camera);
}

void Player::DrawImGui() {
	model_->DrawImGui("PlayerModel");
}