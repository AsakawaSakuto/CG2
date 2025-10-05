#include "Application/GameObject/Player/Player.h"
#include <numbers>

void Player::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/model/player/player.obj");

	transform_.scale = {1.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};
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

	// プレイヤーの上昇
	MovePlayerUpward();

	// プレイヤーの速度を一定の値に収める
	ClampPlayerVelocity();

	// プレイヤーの反転処理
	ReverseIfAboveLimit(-10.0f, 10.0f);

	// カメラのオフセット変更(補間)
	CameraOffsetChange();

	// プレイヤーの回転
	RotateChange();

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();
}

void Player::Draw(Camera useCamera) { model_->Draw(useCamera); }

void Player::DrawImgui() {
	ImGui::Begin("Player Control");

	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f);

	if (ImGui::Button("Reset")) {
		transform_.translate = {0.0f, 0.0f, 0.0f};
		transform_.rotate = {0.0f, 0.0f, 0.0f};
	}

	ImGui::End();
}

void Player::MovePlayerUpward() {
	// 上昇
	velocity_.y += acceleration_.y * deltaTime_;
	transform_.translate.y += velocity_.y * deltaTime_;
}

void Player::ClampPlayerVelocity() {
	// プレイヤーの速度を一定の値に収める
	velocity_.y = std::clamp(velocity_.y, -4.0f, 4.0f);
}

void Player::ReverseIfAboveLimit(float minHeight, float maxHeight) {
	// プレイヤーが最高地点に到達したとき
	if (transform_.translate.y >= maxHeight && direction_ == Direction::UP) {
		// 反転
		//transform_.rotate.z = std::numbers::pi_v<float>;

		// オフセット変更
		isOffsetChange_ = true;

		// プレイヤーの進行方向を徐々に反対方向に
		acceleration_.y *= -1;

		// プレイヤーの進行状況
		direction_ = Direction::DOWN;
	}

	// プレイヤーが最低地点に到達したとき
	if (transform_.translate.y <= minHeight && direction_ == Direction::DOWN) {
		// 反転
		//transform_.rotate.z = 0.0f;

		// オフセット変更
		isOffsetChange_ = true;

		// プレイヤーの進行方向を徐々に反対方向に
		acceleration_.y *= -1;

		// プレイヤーの進行状況
		direction_ = Direction::UP;
	}
}

void Player::CameraOffsetChange() {
	if (!isOffsetChange_)
		return;

	// 方向に応じて目標オフセットを決定
	float targetOffset = (direction_ == Direction::UP) ? 4.0f : -4.0f;

	// 補間係数
	const float smoothing = 0.025f; // 小さいほどゆっくり

	// 補間処理
	cameraOffset_ = std::lerp(cameraOffset_, targetOffset, smoothing);

	// velocityの符号が反転したかチェック
	bool velocityFlipped = (direction_ == Direction::UP && velocity_.y > 0.0f) || (direction_ == Direction::DOWN && velocity_.y < 0.0f);

	if (velocityFlipped) {
		cameraOffset_ = targetOffset;
		isOffsetChange_ = false;
	}
}

void Player::RotateChange() {
	if (!isOffsetChange_)
		return;

	// 目標回転角
	float targetRotationZ = (direction_ == Direction::UP) ? 0.0f : std::numbers::pi_v<float>;

	// 補間係数
	const float smoothing = 0.1f;

	// 補間処理
	transform_.rotate.z = std::lerp(transform_.rotate.z, targetRotationZ, smoothing);

	if (std::abs(transform_.rotate.z - targetRotationZ) < 0.01f) {
		transform_.rotate.z = targetRotationZ;
	}
}
