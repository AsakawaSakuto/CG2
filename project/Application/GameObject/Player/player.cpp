#include "player.h"
#include <cmath>
#include <algorithm>

void Player::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	transform_.scale = { 1.0f,1.0f,1.0f };

	model_->Initialize(&ctx_->dxCommon, "player/player.obj");

	moveParticle_->Initialize(&ctx_->dxCommon);
	moveParticle_->LoadJson("playerMove");
	landingParticle_->Initialize(&ctx_->dxCommon);
	landingParticle_->LoadJson("playerLanding");
}

void Player::Update() {

	Move();
	Jump();

	transform_.translate.x = std::clamp(transform_.translate.x, -49.5f, 49.5f);
	transform_.translate.z = std::clamp(transform_.translate.z, -49.5f, 49.5f);

	model_->SetTransform(transform_);
	model_->Update();

	moveParticle_->SetEmitterPosition(transform_.translate);
	moveParticle_->SetOffSet({ 0.0f, -0.2f, 0.0f });
	moveParticle_->Update();
	landingParticle_->SetEmitterPosition(transform_.translate);
	landingParticle_->Update();
}

void Player::Draw(Camera camera) {
	// カメラを保存（移動計算で使用）
	camera_ = camera;
	model_->Draw(camera);

	moveParticle_->Draw(camera);
	landingParticle_->Draw(camera);
}

void Player::DrawImGui() {

	// プレイヤー固有のImGui
	ImGui::Begin("Player Settings");
	ImGui::DragFloat("Move Speed", &status_.moveSpeed_, 0.1f, 0.1f, 20.0f);
	ImGui::DragFloat3("Position", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.01f);
	
	// ジャンプ設定
	ImGui::Separator();
	ImGui::Text("Jump Settings");
	ImGui::DragInt("Jump Can Count", &status_.jumpCanCount_, 1, 1, 5);
	ImGui::DragFloat("Jump Power", &status_.jumpPower_, 0.1f, 1.0f, 20.0f);
	ImGui::DragFloat("Gravity", &status_.gravity_, 0.1f, 1.0f, 50.0f);
	ImGui::DragFloat("Ground Level", &groundLevel_, 0.1f, -10.0f, 10.0f);
	ImGui::Text("Current Jump Count: %d / %d", status_.currentJumpCount_, status_.jumpCanCount_);
	ImGui::Text("Is Grounded: %s", isGrounded_ ? "Yes" : "No");
	ImGui::Text("Velocity Y: %.2f", status_.velocity_Y_);
	ImGui::End();

	//landingParticle_->DrawImGui("move Particle");
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
			moveDirection.x * status_.moveSpeed_ * deltaTime_,
			0.0f, // Y軸移動は制限
			moveDirection.z * status_.moveSpeed_ * deltaTime_
		};

		// プレイヤーの位置を更新
		transform_.translate.x += movement.x;
		transform_.translate.z += movement.z;

		// プレイヤーの向きを移動方向に合わせる
		if (std::abs(moveDirection.x) > 0.01f || std::abs(moveDirection.z) > 0.01f) {
			float targetYaw = std::atan2(moveDirection.x, moveDirection.z);
			transform_.rotate.y = targetYaw;
		}
		
		if (!moveParticle_->IsPlaying()) {
			moveParticle_->Play();
		}
	} else {
		moveParticle_->Stop();
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
	// stickX > 0: 右に倒す → カメラから見て右方向に移動
	// stickX < 0: 左に倒す → カメラから見て左方向に移動
	Vector3 moveDirection = {
		forward.x * stickY + right.x * stickX,
		0.0f,
		forward.z * stickY + right.z * stickX
	};
	
	return moveDirection.Normalize();
}

void Player::Jump() {
	// 前フレームの地面接触状態を保存
	wasGrounded_ = isGrounded_;

	// 地面にいるかのチェック
	if (transform_.translate.y <= groundLevel_) {
		transform_.translate.y = groundLevel_;
		status_.velocity_Y_ = 0.0f;
		isGrounded_ = true;
		status_.currentJumpCount_ = 0; // 地面に着いたらジャンプカウントをリセット
		
		// 着地した瞬間の判定（前フレームで空中にいて、今フレームで地面に接触）
		if (!wasGrounded_) {
			// 着地した瞬間の処理
			landingParticle_->Play(false);
		}
	} else {
		isGrounded_ = false;
	}

	// Aボタンでジャンプ
	if (ctx_->gamePad.TriggerButton(GamePad::A)) {
		if (status_.currentJumpCount_ < status_.jumpCanCount_) {
			status_.velocity_Y_ = status_.jumpPower_;
			status_.currentJumpCount_++;
		}
	}

	// 重力を適用
	if (!isGrounded_ || status_.velocity_Y_ > 0.0f) {
		status_.velocity_Y_ -= status_.gravity_ * deltaTime_;
		transform_.translate.y += status_.velocity_Y_ * deltaTime_;
	}
}