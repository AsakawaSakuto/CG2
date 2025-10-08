#include "Application/GameObject/Player/Player.h"
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

	// 弾をためる
	BulletCharge();

	// 弾を発射する
	BulletShot();

	// 弾の更新
	BulletUpdate();

	// スタン解除
	StunRemoved();

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();
}

void Player::Draw(Camera useCamera) { 
	// プレイヤー描画
	if (stunTimer_ % 2 == 0) {
		model_->Draw(useCamera);
	}

	// 弾の描画
	for (auto& bullet : bullets_) {
		bullet->Draw(useCamera);
	}
}

void Player::DrawImgui() {
	// プレイヤーのImGui
	PlayerImGui();

	// 弾のImGui
	BulletImGui();
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
	state_.cameraOffset = std::lerp(state_.cameraOffset, targetOffset, smoothing);

	// velocityの符号が反転したかチェック
	bool velocityFlipped = (direction_ == Direction::UP && velocity_.y > 0.0f) || (direction_ == Direction::DOWN && velocity_.y < 0.0f);

	if (velocityFlipped) {
		state_.cameraOffset = targetOffset;
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

	if (input_->TriggerKey(DIK_SPACE)) {
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
			        return y > 50.0f || y < -50.0f; // 高さの上限50　下限-50
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
	ImGui::DragInt("Stun Duration", &state_.stunDuration, 1, 0, 600);

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
	velocity_.y += (direction_ == Direction::UP ? -0.5f : 0.5f); 
}

void Player::Stun() { 
	// 敵にヒットしたら減速
	if (!isStun_) {
		isStun_ = true;
		stunTimer_ = state_.stunDuration;

		// 減速
		SpeedDown();
	}
}

void Player::StunRemoved() {
	if (isStun_) {
		stunTimer_--;
		if (stunTimer_ <= 0) {
			isStun_ = false;
			stunTimer_ = 0;
		}
		return;
	}
}
