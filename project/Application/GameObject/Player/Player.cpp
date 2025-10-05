#include "Application/GameObject/Player/Player.h"

void Player::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/model/player/player.obj");

	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,0.0f,0.0f };
	CollitionSphere_.center = transform_.translate;
	CollitionSphere_.radius = 1.0f;

	// 速度関連初期化
	acceleration_ = {0.0f, 0.98f};
	velocity_ = {};
}

void Player::Update() {

	// 当たり判定用の球の中心を更新
	CollitionSphere_.center = transform_.translate;

	if (input_->PushKey(DIK_LEFT)) {
		transform_.translate.x -= 5.0f * deltaTime_;
	}

	if (input_->PushKey(DIK_RIGHT)) {
		transform_.translate.x += 5.0f * deltaTime_;
	}

	// 上昇
	velocity_.y += acceleration_.y * deltaTime_;
	transform_.translate.y += velocity_.y * deltaTime_;

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();
}

void Player::Draw(Camera useCamera) {
	model_->Draw(useCamera);
}

void Player::DrawImgui()
{
	ImGui::Begin("Player Control");

	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);

	if (ImGui::Button("Reset")) {
		transform_.translate = {0.0f, 0.0f, 0.0f};
		transform_.rotate = {0.0f, 0.0f, 0.0f};
	}

	ImGui::End();
}