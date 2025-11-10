#include "player.h"
#include <cmath>
#include <algorithm>

void Player::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	model_->Initialize(&ctx_->dxCommon, "player/player.obj");
	transform_.scale = { 1.0f,1.0f,1.0f };
	// 移動速度を設定
	moveSpeed_ = 5.0f;
}

void Player::Update() {

	Move();

	model_->SetTransform(transform_);
	model_->Update();
}

void Player::Draw(Camera camera) {
	// カメラを保存（移動計算で使用）
	camera_ = camera;
	model_->Draw(camera);
}

void Player::DrawImGui() {
	model_->DrawImGui("PlayerModel");
	
	// プレイヤー固有のImGui
	ImGui::Begin("Player Settings");
	ImGui::DragFloat("Move Speed", &moveSpeed_, 0.1f, 0.1f, 20.0f);
	ImGui::DragFloat3("Position", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.01f);
}

void Player::Move() {
	// 左スティックの入力を取得
	float leftStickX = ctx_->gamePad.LeftStickX();
	float leftStickY = ctx_->gamePad.LeftStickY();

	// スティックの入力があるかチェック
	if (std::abs(leftStickX) > 0.01f || std::abs(leftStickY) > 0.01f) {
		// カメラから移動方向ベクトルを計算
		Vector3 moveDirection = CalculateCameraMoveDirection(leftStickX, leftStickY);

		// 移動量を計算
		Vector3 movement = {
			moveDirection.x * moveSpeed_ * deltaTime_,
			0.0f, // Y軸移動は制限
			moveDirection.z * moveSpeed_ * deltaTime_
		};

		// プレイヤーの位置を更新
		transform_.translate.x += movement.x;
		transform_.translate.z += movement.z;

		// プレイヤーの向きを移動方向に合わせる
		if (std::abs(moveDirection.x) > 0.01f || std::abs(moveDirection.z) > 0.01f) {
			float targetYaw = std::atan2(moveDirection.x, moveDirection.z);
			transform_.rotate.y = targetYaw;
		}
	}
}

Vector3 Player::CalculateCameraMoveDirection(float stickX, float stickY) {
	
	// カメラとプレイヤーの位置を取得
	Vector3 cameraPos = camera_.GetWorldPosition();
	Vector3 playerPos = transform_.translate;
	
	// プレイヤーからカメラへの方向ベクトル
	Vector3 playerToCamera = {
		cameraPos.x - playerPos.x,
		0.0f, // Y成分は無視
		cameraPos.z - playerPos.z
	};
	
	// 正規化してカメラ方向ベクトルにする
	Vector3 cameraDirection = playerToCamera.Normalize();
	
	// カメラの向いている方向 = カメラ方向の逆
	Vector3 forward = {
		-cameraDirection.x,  // カメラ方向の逆
		0.0f,                // Y成分は無視
		-cameraDirection.z   // カメラ方向の逆
	};
	
	// 右方向は前方向を90度時計回りに回転
	Vector3 right = {
		forward.z,   // 前方のZ成分を右方向のX成分に
		0.0f,        // Y成分は無視
		-forward.x   // 前方のX成分の逆を右方向のZ成分に
	};
	
	// スティック入力に応じて移動方向を計算
	// stickY > 0: 上に倒す → カメラが向いている方向（前方向）に移動
	// stickY < 0: 下に倒す → カメラと反対方向（後方向）に移動  
	// stickX > 0: 右に倒す → 카メラから見て右方向に移動
	// stickX < 0: 左に倒す → 카メラから見て左方向に移動
	Vector3 moveDirection = {
		forward.x * stickY + right.x * stickX,
		0.0f,
		forward.z * stickY + right.z * stickX
	};
	
	return moveDirection.Normalize();
}