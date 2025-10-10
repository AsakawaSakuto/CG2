#define NOMINMAX
#include "Application/GameObject/Player/Player.h"
#include "Application/GameObject/Block/Block.h"
#include "Application/GameObject/Thorn/Thorn.h"
#include <numbers>

#include "State/PlayerStateLoader.h"

void Player::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "resources/model/player/player.obj");

	// JSONからステータスを読み込み
	state_ = PlayerStateLoader::Load("Resources/Data/playerState.json");

	transform_.scale = {1.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};
	collisionSphere_.center = transform_.translate;
	collisionSphere_.radius = 1.0f;

	// プレイヤーの羽の初期化
	playerWing_->Initialize(dxCommon_);
	playerWing_->SetPosition(transform_.translate);

	// 速度関連初期化
	acceleration_ = {0.0f, 1.5f, 0.0f};
	velocity_ = {7.0f, 0.0f};
}

void Player::Update() {

	// 当たり判定用の球の中心を更新
	collisionSphere_.center = transform_.translate;

	bool isLeftMove = gamePad_->LeftStickX() <= -0.3f || gamePad_->PushButton(gamePad_->DPAD_LEFT) || input_->PushKey(DIK_LEFT);
	bool isRightMove = gamePad_->LeftStickX() >= 0.3f || gamePad_->PushButton(gamePad_->DPAD_RIGHT) || input_->PushKey(DIK_RIGHT);

	if (isLeftMove) {
		transform_.translate.x -= velocity_.x * deltaTime_;
	}

	if (isRightMove) {
		transform_.translate.x += velocity_.x * deltaTime_;
	}

	// プレイヤーの羽の位置をプレイヤーに合わせる
	playerWing_->SetPosition(transform_.translate);

	// プレイヤーの羽の更新
	playerWing_->Update();

	// プレイヤーの上昇
	MovePlayerUpward();

	// プレイヤーの速度を一定の値に収める
	ClampPlayerVelocity();

	// プレイヤーの反転処理
	ReverseIfAboveLimit(START_LINE, END_LINE);

	// カメラのオフセット変更(補間)
	CameraOffsetChange();

	// プレイヤーの回転
	RotateChange();

	// 弾をためる
	BulletCharge();

	// 弾を発射する
	BulletShot();

	// 弾の更新
	BulletUpdate();

	// スタン解除
	StunRemoved();

	// トゲとの当たり判定
	ThornCollision();

	// ブロックとの当たり判定
	BlockCollision();

	// 弾とトゲの当たり判定
	BulletThornCollison();

	// カメラシェイクの値を更新
	UpdateCameraShake();

	// プレイヤーの羽の状態更新
	WingStateUpdate();

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();
}

void Player::Draw(Camera useCamera) {
	// プレイヤー描画
	model_->Draw(useCamera);

	// 弾の描画
	for (auto& bullet : bullets_) {
		bullet->Draw(useCamera);
	}

	// プレイヤーの羽の描画
	playerWing_->Draw(useCamera);
}

void Player::DrawImgui() {
	// プレイヤーのImGui
	PlayerImGui();

	// 弾のImGui
	//BulletImGui();

	ImGui::Begin("test");

	ImGui::Text("shakeAmountX : %f", shakeAmount_.x);
	ImGui::Text("shakeAmountY : %f", shakeAmount_.y);
	ImGui::Text("isShake : %d", isShake_);

	ImGui::End();
}

void Player::MovePlayerUpward() {
	// 上昇
	velocity_.y += acceleration_.y * deltaTime_;
	transform_.translate.y += velocity_.y * deltaTime_;
}

void Player::ClampPlayerVelocity() {
	// プレイヤーの速度を一定の値に収める
	velocity_.y = std::clamp(velocity_.y, -state_.maxSpeed, state_.maxSpeed);
}

void Player::ReverseIfAboveLimit(float minHeight, float maxHeight) {
	// プレイヤーが最高地点に到達したとき
	if (transform_.translate.y >= maxHeight && direction_ == Direction::UP) {
		// プレイヤーの進行方向を徐々に反対方向に
		acceleration_.y *= -1;

		// プレイヤーの進行状況
		direction_ = Direction::DOWN;
	}

	// プレイヤーが最低地点に到達したとき
	if (transform_.translate.y <= minHeight && direction_ == Direction::DOWN) {
		// プレイヤーの進行方向を徐々に反対方向に
		acceleration_.y *= -1;

		// プレイヤーの進行状況
		direction_ = Direction::UP;
	}
}

void Player::CameraOffsetChange() {
	// 方向に応じて目標オフセットを決定
	float targetOffset = (direction_ == Direction::UP) ? CAMERA_OFFSET_TOP : CAMERA_OFFSET_BOTTOM;

	// velocityの符号が反転したかチェック
	bool velocityFlipped = (direction_ == Direction::UP && velocity_.y > 0.0f) || (direction_ == Direction::DOWN && velocity_.y < 0.0f);

	// 徐々にオフセットを目標値に近づける
	if (velocityFlipped) {
		state_.cameraOffset += velocity_.y * deltaTime_;
	}

	 state_.cameraOffset = std::clamp(state_.cameraOffset, CAMERA_OFFSET_BOTTOM, CAMERA_OFFSET_TOP);
}

void Player::RotateChange() {
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

void Player::BulletCharge() {
	// ゲージが最大値なら早期リターン
	if (bulletGauge_ >= state_.bulletGaugeMax) {
		return;
	}

	///////////// 仮の処理 /////////////
	num_++;

	// 2秒に一回ゲージをためる
	if (num_ >= 120) {
		bulletGauge_++;
		num_ = 0;
	}
	///////////// 仮の処理 /////////////
}

void Player::BulletShot() {
	if (bulletGauge_ <= 0) {
		return;
	}

	bool isShot = gamePad_->TriggerButton(gamePad_->A) || input_->TriggerKey(DIK_SPACE);

	if (isShot) {
		// 弾の生成
		auto bullet = std::make_unique<Bullet>();
		bullet->Initialize(dxCommon_);

		// プレイヤーの向いてる方向に応じて弾の進む向きも決まる
		if (direction_ == Direction::UP) {
			bullet->Spawn(transform_.translate, bullet->GetSpeed());
		} else {
			bullet->Spawn(transform_.translate, -bullet->GetSpeed());
		}

		bullets_.push_back(std::move(bullet));

		// プレイヤー減速
		SpeedDown();

		// ゲージを減らす
		--bulletGauge_;
	}
}

void Player::BulletUpdate() {
	// 弾の更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// 弾の削除
	if (!bullets_.empty()) {
		bullets_.erase(
		    std::remove_if(
		        bullets_.begin(), bullets_.end(),
		        [](const std::unique_ptr<Bullet>& bullet) {
			        float y = bullet->GetTransform().translate.y;

					//////////////// 変更予定 ////////////////
			        return y > 100.0f || y < -100.0f; // 高さの上限100　下限-100
					//////////////// 変更予定 ////////////////

		        }),
		    bullets_.end());
	}
}

void Player::PlayerImGui() {
	// プレイヤーのImGui
	ImGui::Begin("Player Control");

	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f);
	ImGui::Text("BulletGauge: %d", bulletGauge_);
	ImGui::Text("Bullet Count: %d", static_cast<int>(bullets_.size()));

	if (ImGui::Button("Reset")) {
		transform_.translate = {0.0f, 0.0f, 0.0f};
		transform_.rotate = {0.0f, 0.0f, 0.0f};
		velocity_.y = 0.0f;
		bulletGauge_ = 0;
	}

	ImGui::End();
}

void Player::DrawImGuiJsonState() {
	ImGui::Begin("Player State");

	// --- JSONから読み込んだパラメータを直接編集 ---
	ImGui::DragFloat("Max Speed", &state_.maxSpeed, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Camera Offset", &state_.cameraOffset, 0.01f, -20.0f, 20.0f);
	ImGui::DragInt("Bullet Gauge Max", &state_.bulletGaugeMax, 1, 1, 100);
	ImGui::DragFloat("Stun Duration", &state_.stunDuration, 0.1f, 0.0f, 60.0f);
	ImGui::DragFloat("SpeedDown Strength", &state_.speedDownStrength, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("Camera ShakeStrength", &state_.shakeStrength, 0.01f, 0.0f, 2.0f);

	// --- JSONへ保存ボタン ---
	if (ImGui::Button("Save JSON")) {
		PlayerStateLoader::Save("Resources/Data/playerState.json", state_);
	}

	ImGui::End();
}

void Player::BulletImGui() {
	// 弾のImGui
	ImGui::Begin("Bullet");

	/*if (!bullets_.empty()) {
	    ImGui::DragFloat3("Translate", &bullets_[0]->GetTransform().translate.x, 0.01f);
	}*/

	ImGui::End();
}

void Player::SpeedDown() {
	// 減速
	velocity_.y += (direction_ == Direction::UP ? -state_.speedDownStrength : state_.speedDownStrength);
}

void Player::Stun() {
	// 敵にヒットしたら減速
	if (!stunTimer_.IsActive()) {
		stunTimer_.Start(state_.stunDuration, false);

		// 減速
		SpeedDown();
	}
}

void Player::StunRemoved() { stunTimer_.Update(); }

void Player::ThornCollision() {
	for (auto& thorn : thorns_) {
		if (Collision::IsHit(thorn->GetCollitionSphere(), collisionSphere_)) {
			if (direction_ == Direction::DOWN) {
				// エサをばら撒く

				// スタン
				// Stun();

				// シェイク用のフラグを立てる
				if (!isShake_) {
					isShake_ = true;
				}

				// トゲを非アクティブにする
				thorn->SetIsAlive(false);
				break;
			} else if (direction_ == Direction::UP) {
				// スタン
				Stun();
				break;
			}
		}
	}
}

void Player::BlockCollision() {
	for (auto& block : blocks_) {
		if (Collision::IsHit(block->GetCollitionSphere(), collisionSphere_)) { 
			// ブロックを非アクティブにする
			block->SetIsAlive(false);
			break;
		}
	}
}

void Player::BulletThornCollison() {
	for (auto& bullet : bullets_) {
		for (auto& thorn : thorns_) {
			if (Collision::IsHit(bullet->GetCollitionSphere(), thorn->GetCollitionSphere())) {
				// トゲを強化する
				switch (thorn->GetThornType()) {
				case ThornType::MIN:
					thorn->SetThornType(ThornType::MIDDLE);
					break;
				case ThornType::MIDDLE:
					thorn->SetThornType(ThornType::MAX);
				}
			}
		}
	}
}

void Player::UpdateCameraShake() {
	float shakeDecayRate_ = 2.0f; // 揺れの減衰速度

	if (isShake_) {
		state_.shakeStrength -= shakeDecayRate_ * deltaTime_;
		state_.shakeStrength -= shakeDecayRate_ * deltaTime_;

		state_.shakeStrength = std::max(0.0f, state_.shakeStrength);
		state_.shakeStrength = std::max(0.0f, state_.shakeStrength);

		shakeAmount_.x = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * state_.shakeStrength;
		shakeAmount_.y = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * state_.shakeStrength;

		if (state_.shakeStrength <= 0.01f) {
			isShake_ = false;
			shakeAmount_ = {0.0f, 0.0f};
			state_.shakeStrength = 0.5f;
		}
	}
}

void Player::WingStateUpdate() {
	if (direction_ == Direction::UP) {
		playerWing_->SetIsAlive(true);
	} else if (direction_ == Direction::DOWN) {
		playerWing_->SetIsAlive(false);
	}
}
