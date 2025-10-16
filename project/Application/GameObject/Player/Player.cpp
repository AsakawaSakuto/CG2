#define NOMINMAX
#include "Application/GameObject/Player/Player.h"
#include "Application/GameObject/Block/Block.h"
#include "Application/GameObject/Thorn/Thorn.h"
#include <numbers>

// #include "State/PlayerStateLoader.h"
#include "Application/GameObject/State/JsonState.h"

void Player::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "player/player.obj");
	model_->SetTexture("resources/image/uvChecker0.5.png");

	// JSONからステータスを読み込み
	// state_ = PlayerStateLoader::Load("Resources/Data/playerState.json");
	playerState_ = JsonState::Load<PlayerState>("Resources/Data/playerState.json");
	bulletState_ = JsonState::Load<BulletState>("Resources/Data/bulletState.json");

	transform_.scale = {1.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};
	collisionSphere_.center = transform_.translate;
	collisionSphere_.radius = 1.0f;

	// 当たり判定更新(AABB)
	UpdateCollisionAABB();

	// プレイヤーの羽の初期化
	playerWing_->Initialize(dxCommon_);
	playerWing_->SetPosition(transform_.translate);

	// 速度関連初期化
	acceleration_ = {0.0f, 1.5f, 0.0f};
	velocity_ = {7.0f, 0.0f};

	// スコア
	score_ = 0;

	// 弾のゲージ
	bulletGauge_ = 0;

	// ゴールフラグ初期化
	isGoal_ = false;

	// カメラのオフセット初期化
	playerState_.cameraOffset = CAMERA_OFFSET_TOP;

	// プレイヤーの方向初期化
	direction_ = Direction::UP;
}

void Player::Update() {

	// 当たり判定用の球の中心を更新
	collisionSphere_.center = transform_.translate;

	bool isLeftMove = gamePad_->LeftStickX() <= -0.3f || gamePad_->PushButton(gamePad_->DPAD_LEFT) || input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A);
	bool isRightMove = gamePad_->LeftStickX() >= 0.3f || gamePad_->PushButton(gamePad_->DPAD_RIGHT) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D);

	if (isLeftMove) {
		transform_.translate.x -= velocity_.x * deltaTime_;
	}

	if (isRightMove) {
		transform_.translate.x += velocity_.x * deltaTime_;
	}

	// プレイヤーの移動制限
	PlayerMoveLimit();

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
	CollisionThorn();

	// ブロックとの当たり判定
	CollisionBlock();

	// 弾とトゲの当たり判定
	CollisonBulletThorn();

	// カメラシェイクの値を更新
	UpdateCameraShake();

	// プレイヤーの羽の状態更新
	WingStateUpdate();

	// トゲのクールダウン更新
	TickThornCooldown();

	// プレイヤーの羽とトゲの当たり判定
	CollisionWingThorn();

	// 羽のクールダウンフレーム加算
	WingCoolDownFramesAdd();

	// 当たり判定更新(AABB)
	UpdateCollisionAABB();

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();
}

void Player::Draw(Camera useCamera) {
	// プレイヤー描画
	if (stunTimer_.GetCurrentFrame() % 2 == 0) {
		model_->Draw(useCamera);
	}

	// 弾の描画
	for (auto& bullet : bullets_) {
		bullet->Draw(useCamera);
	}

	// プレイヤーの羽の描画
	if (!isStartCoolDown_) {
		playerWing_->Draw(useCamera);
	}
}

void Player::DrawImgui() {
	// プレイヤーのImGui
	PlayerImGui();

	// スコアのImgui
	ScoreImGui();

	// プレイヤーのステータス
	DrawImGuiJsonStatePlayer();

	// 弾のステータス
	DrawImGuiJsonStateBullet();
}

void Player::SetBulletGaugeSprites(std::array<BulletGaugeInfo, 5>* gaugeSprites) { bulletGaugeSprites_ = gaugeSprites; }

void Player::MovePlayerUpward() {
	// 上昇
	velocity_.y += acceleration_.y * deltaTime_;
	transform_.translate.y += velocity_.y * deltaTime_;
}

void Player::ClampPlayerVelocity() {
	// プレイヤーの速度を一定の値に収める
	velocity_.y = std::clamp(velocity_.y, -playerState_.maxSpeed, playerState_.maxSpeed);
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
		// acceleration_.y *= -1;

		// プレイヤーの進行状況
		// direction_ = Direction::UP;

		// ゴールフラグをたてる
		isGoal_ = true;

		// プレイヤーのステータス初期化
		transform_.rotate.z = 0.0f;
		direction_ = Direction::UP;
		playerState_.cameraOffset = CAMERA_OFFSET_BOTTOM;
	}
}

void Player::CameraOffsetChange() {
	// 方向に応じて目標オフセットを決定
	float targetOffset = (direction_ == Direction::UP) ? CAMERA_OFFSET_TOP : CAMERA_OFFSET_BOTTOM;

	// velocityの符号が反転したかチェック
	bool velocityFlipped = (direction_ == Direction::UP && velocity_.y > 0.0f) || (direction_ == Direction::DOWN && velocity_.y < 0.0f);

	// 徐々にオフセットを目標値に近づける
	if (velocityFlipped) {
		playerState_.cameraOffset += velocity_.y * deltaTime_;
	}

	playerState_.cameraOffset = std::clamp(playerState_.cameraOffset, CAMERA_OFFSET_BOTTOM, CAMERA_OFFSET_TOP);
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
	if (bulletGauge_ >= bulletState_.bulletGaugeMax) {
		return;
	}

	///////////// 仮の処理 /////////////
	num_++;

	// 2秒に一回ゲージをためる
	if (num_ >= 120) {
		bulletGauge_++;
		(*bulletGaugeSprites_)[bulletGauge_ - 1].isActive = true;
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
			bullet->Spawn(transform_.translate, bulletState_.maxSpeed);
		} else {
			bullet->Spawn(transform_.translate, -bulletState_.maxSpeed);
		}

		bullets_.push_back(std::move(bullet));

		// プレイヤー減速
		SpeedDown(playerState_.speedDownStrengthBullet);

		// ゲージを減らす
		--bulletGauge_;

		// 描画用のフラグを下ろす
		(*bulletGaugeSprites_)[bulletGauge_].isActive = false;
	}
}

void Player::BulletUpdate() {
	// 弾の更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	float playerPosY = transform_.translate.y;

	// 弾の削除
	if (!bullets_.empty()) {
		bullets_.erase(
		    std::remove_if(
		        bullets_.begin(), bullets_.end(),
		        [playerPosY](const std::unique_ptr<Bullet>& bullet) { // playerPosYをキャプチャ
			        float y = bullet->GetTransform().translate.y;
			        return y > playerPosY + 11.0f || y < playerPosY - 11.0f; // 画面外に出たら削除
		        }),
		    bullets_.end());
	}
}

void Player::PlayerImGui() {
	// プレイヤーのImGui
	ImGui::Begin("Player Control");

	// 座標
	ShowLabeledVector3("Translate", &transform_.translate.x);

	// 回転
	ShowLabeledVector3("Rotate", &transform_.rotate.x);

	// 大きさ
	ShowLabeledVector3("Scale", &transform_.scale.x);

	// 速度
	ShowLabeledVector3("Velocity", &velocity_.x);

	// 弾のステータス
	ImGui::Text("BulletGauge: %d", bulletGauge_);
	ImGui::Text("Bullet Count: %d", static_cast<int>(bullets_.size()));

	// リセットボタン
	if (ImGui::Button("Reset")) {
		transform_.translate = {0.0f, 0.0f, 0.0f};
		transform_.rotate = {0.0f, 0.0f, 0.0f};
		velocity_.y = 0.0f;
		bulletGauge_ = 0;
	}

	ImGui::End();
}

void Player::DrawImGuiJsonStatePlayer() {
	ImGui::Begin("Player State");

	nlohmann::json jsonState = playerState_;

	// JsonNo中身をImGuiで表示する
	DrawImGuiForJson(jsonState);

	playerState_ = jsonState.get<PlayerState>();

	// --- JSONへ保存ボタン ---
	if (ImGui::Button("Save JSON")) {
		JsonState::Save("Resources/Data/playerState.json", playerState_);
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

void Player::SpeedDown(float speedDpwnStrength) {
	// 減速
	velocity_.y += (direction_ == Direction::UP ? -speedDpwnStrength : speedDpwnStrength);
}

void Player::Stun() {
	// 敵にヒットしたら減速
	if (!stunTimer_.IsActive()) {
		stunTimer_.Start(playerState_.stunDuration, false);

		// 減速
		SpeedDown(playerState_.speedDownStrengthThorn);

		// 回転
		StunRotate();
	}
}

void Player::StunRemoved() { stunTimer_.Update(); }

void Player::CollisionThorn() {
	for (auto& thorn : thorns_) {
		if (Collision::IsHit(thorn->GetCollisionAABB(), collisionAABB_) && thorn->GetIsAlive()) {
			if (direction_ == Direction::DOWN) {


				// シェイク用のフラグを立てる
				if (!isShake_) {
					isShake_ = true;
				}

				// スコア加算
				AddScore(thorn->GetScoreAmount());

				// トゲを非アクティブにする
				thorn->PlayParticle(5);
				thorn->SetIsAlive(false);

				break;
			} else if (direction_ == Direction::UP) {

				// シェイク用のフラグを立てる
				if (!isShake_ && !isStartCoolDown_) {
					isShake_ = true;
				}

				// 弾のゲージリセット
				ResetBulletGauge();

				for (int i = 0; i < bulletGaugeSprites_->size(); ++i) {
					(*bulletGaugeSprites_)[i].isActive = false;
				}

				// スタン
				Stun();

				// 羽のクールダウン開始
				isStartCoolDown_ = true;
				break;
			}
		}
	}
}

void Player::CollisionBlock() {
	for (auto& block : blocks_) {
		if (Collision::IsHit(block->GetCollitionSphere(), collisionSphere_) && block->GetIsAlive()) {
			// シェイク用のフラグを立てる
			if (!isShake_) {
				isShake_ = true;
			}

			// ブロックを非アクティブにする
			block->SetIsAlive(false);

			// スコア加算
			AddScore(block->GetScoreAmount());
			break;
		}
	}
}

void Player::CollisonBulletThorn() {
	for (auto& bullet : bullets_) {
		for (auto& thorn : thorns_) {
			if (!thorn->CanUpgradeBullet()) {
				continue; // クールダウン中のトゲはスキップ
			}

			if (Collision::IsHit(bullet->GetCollitionSphere(), thorn->GetCollitionSphere())) {
				switch (thorn->GetThornType()) {
				case ThornType::MIN:
					thorn->SetThornType(ThornType::MIDDLE);
					thorn->SetUpgradeCooldownBullet(10); // 10フレームのクールダウン
					break;
				case ThornType::MIDDLE:
					thorn->SetThornType(ThornType::MAX);
					thorn->SetUpgradeCooldownBullet(10);
					break;
				default:
					break;
				}
			}
		}
	}
}

void Player::UpdateCameraShake() {
	float shakeDecayRate_ = 2.0f; // 揺れの減衰速度

	if (isShake_) {
		playerState_.shakeStrength -= shakeDecayRate_ * deltaTime_;
		playerState_.shakeStrength -= shakeDecayRate_ * deltaTime_;

		playerState_.shakeStrength = std::max(0.0f, playerState_.shakeStrength);
		playerState_.shakeStrength = std::max(0.0f, playerState_.shakeStrength);

		shakeAmount_.x = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * playerState_.shakeStrength;
		shakeAmount_.y = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * playerState_.shakeStrength;

		if (playerState_.shakeStrength <= 0.01f) {
			isShake_ = false;
			shakeAmount_ = {0.0f, 0.0f};
			playerState_.shakeStrength = 0.5f;
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

void Player::AddScore(int score) { score_ += score; }

void Player::ScoreImGui() {
	ImGui::Begin("Score");

	ImGui::Text("TotalScore : %f", score_);
	ImGui::Text("distance : %f", dis);

	ImGui::End();
}

void Player::TickThornCooldown() {
	for (auto& thorn : thorns_) {
		thorn->TickCooldown();
	}
}

void Player::CollisionWingThorn() {
	for (auto& thorn : thorns_) {
		if (!thorn->CanUpgradeWing()) {
			continue; // トゲがクールダウン中のときはスキップ
		}

		if (currentCoolDownFrames_ > 0) {
			continue; // 羽がクールダウン中のときはスキップ
		}

		if (Collision::IsHit(thorn->GetCollisionAABB(), playerWing_->GetCollisionAABB()) && playerWing_->GetIsAlive()) {

			// 羽とトゲの距離に応じてスコア加算
			Vector3 hitPos = thorn->GetPosition();
			dis = (hitPos - transform_.translate).Length(); // 3Dベクトルの距離

			if (dis < kNearThreshold) {
				AddScoreByDistance(thorn, kNearScore); // 近距離スコア

				///////////////// デバッグ用 /////////////////
				thorn->GetModel()->SetColor({0.0f, 0.0f, 1.0f, 1.0f});
				///////////////// デバッグ用 /////////////////
			} else {
				AddScoreByDistance(thorn, kFarScore); // 遠距離スコア

				///////////////// デバッグ用 /////////////////
				thorn->GetModel()->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
				///////////////// デバッグ用 /////////////////
			}

			thorn->SetUpgradeCooldownWing(10); // 10フレームのクールダウン

			break;
		}
	}
}

void Player::ResetBulletGauge() { bulletGauge_ = 0; }

void Player::AddScoreByDistance(std::shared_ptr<Thorn>& thorn, int scoreAmount) {
	switch (thorn->GetThornType()) {
	case ThornType::MIN:
		AddScore(scoreAmount); // 等倍
		break;
	case ThornType::MIDDLE:
		AddScore(scoreAmount * 2); // 2倍
		break;
	case ThornType::MAX:
		AddScore(scoreAmount * 3); // 3倍
		break;
	default:
		break;
	}
}

void Player::ShowLabeledVector3(const char* label, float* vec) {
	ImGui::Text("%s", label);
	ImGui::PushID(label);

	ImGui::PushItemWidth(60);
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::DragFloat("##X", &vec[0], 0.1f);
	ImGui::SameLine();
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vec[1], 0.1f);
	ImGui::SameLine();
	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &vec[2], 0.1f);

	ImGui::PopItemWidth();
	ImGui::PopID();
}

void Player::DrawImGuiForJson(nlohmann::json& json) {
	for (auto& [key, value] : json.items()) {
		if (value.is_number_float()) {
			float val = value.get<float>();
			if (ImGui::DragFloat(key.c_str(), &val, 0.01f))
				value = val;
		} else if (value.is_number_integer()) {
			int val = value.get<int>();
			if (ImGui::DragInt(key.c_str(), &val))
				value = val;
		} else if (value.is_string()) {
			std::string str = value.get<std::string>();
			char buf[256];
			strncpy_s(buf, str.c_str(), sizeof(buf));
			if (ImGui::InputText(key.c_str(), buf, sizeof(buf)))
				value = std::string(buf);
		} else if (value.is_object()) {
			if (ImGui::TreeNode(key.c_str())) {
				DrawImGuiForJson(value);
				ImGui::TreePop();
			}
		} else if (value.is_boolean()) {
			bool val = value.get<bool>();
			if (ImGui::Checkbox(key.c_str(), &val))
				value = val;
		}
	}
}

void Player::DrawImGuiJsonStateBullet() {
	ImGui::Begin("Bullet State");

	nlohmann::json jsonState = bulletState_;

	// JsonNo中身をImGuiで表示する
	DrawImGuiForJson(jsonState);

	bulletState_ = jsonState.get<BulletState>();

	// --- JSONへ保存ボタン ---
	if (ImGui::Button("Save JSON")) {
		JsonState::Save("Resources/Data/bulletState.json", bulletState_);
	}

	ImGui::End();
}

void Player::WingCoolDownFramesAdd() {
	if (isStartCoolDown_) {
		currentCoolDownFrames_++;
	}

	if (currentCoolDownFrames_ >= playerState_.maxCoolDownWing) {
		isStartCoolDown_ = false;
		currentCoolDownFrames_ = 0;
	}
}

void Player::PlayerMoveLimit() {
	if (transform_.translate.x >= 4.5f) {
		transform_.translate.x = 4.5f;
	}

	if (transform_.translate.x <= -4.5f) {
		transform_.translate.x = -4.5f;
	}
}

void Player::UpdateCollisionAABB() {
	Vector3 t = transform_.translate;
	collisionAABB_.max = {t.x + 0.3f, t.y + 1.0f, t.z + 1.0f};
	collisionAABB_.min = {t.x - 0.3f, t.y - 1.0f, t.z - 1.0f};
}

void Player::StunRotate() { 
	// 2回転
	transform_.rotate.z += 4.0f * std::numbers::pi_v<float>; 
}
