#define NOMINMAX
#include "Application/GameObject/Player/Player.h"
#include "Application/GameObject/Thorn/Thorn.h"
#include <numbers>

#include "Application/GameObject/State/JsonState.h"
#include "Engine/System/Audio/MasterVolume.h"

#include "Engine/System/DirectXCommon/ExeColor.h"

void Player::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "Machine/Body.obj");
	model_->SetTexture("resources/model/Machine/Machine.png");
	model_->SetColor({ 0.4f, 0.7f, 0.9f, 1.0f });
	model_->SetUpdateFrustumCulling(false);

	// JSONからステータスを読み込み
	playerState_ = JsonState::Load<PlayerState>("Resources/Data/playerState.json");
	bulletState_ = JsonState::Load<BulletState>("Resources/Data/bulletState.json");
	scoreList_ = JsonState::Load<ScoreList>("resources/Data/scoreList.json");

	transform_.scale = { 3.0f, 3.0f, 3.0f };
	transform_.rotate = { 0.0f, std::numbers::pi_v<float>, 0.0f };
	transform_.translate = { 0.0f, -10.0f, 0.0f };
	collisionSphere_.center = transform_.translate;
	collisionSphere_.radius = 0.8f;

	// 当たり判定更新(AABB)
	UpdateCollisionAABB();

	// プレイヤーの羽の初期化
	playerWing_->Initialize(dxCommon_);
	playerWing_->SetPosition(transform_.translate);

	// 速度関連初期化
	acceleration_ = { 35.0f, 2.0f, 0.0f };
	velocity_ = { 0.0f, 0.0f };

	// スコア
	score_ = 0;

	// 弾のゲージ
	bulletGauge_ = 0;

	// ゴールフラグ初期化
	isGoal_ = false;

	// カメラのオフセット初期化
	playerState_.cameraOffset = CAMERA_OFFSET_TOP;

	// プレイヤーの方向初期化
	direction_ = PlayerDirection::UP;

	// クマ
	bear_->Initialize(dxCommon_);

	// カメラ追従フラグ
	isCameraSet_ = false;

	// SE
	shotSE_->Initialize("resources/sound/SE/InGame/ShotSE.mp3");
	playerDamageSE_->Initialize("resources/sound/SE/InGame/PlayerDamageSE.mp3");
	DestroyEnemySE_->Initialize("resources/sound/SE/InGame/AttackEnemySE.mp3");
	gaugeChargeSE_->Initialize("resources/sound/SE/InGame/GaugeChargeSE.mp3");
	getItemSE_->Initialize("resources/sound/SE/InGame/GetItemSE.mp3");
	attackEnemySE_->Initialize("resources/sound/SE/InGame/AttackEnemySE.mp3");

	InitParticle();

	// 点滅用変数
	isFlicker_ = false;
	currentFlickFrames_ = 0;

	// カウントダウンが0になったかどうか
	isCountDownZero_ = false;

	// 数字のスプライト集
	for (int i = 0; i <= 9; ++i) {
		spriteNumCollection_[i] = "resources/image/UI/Number" + std::to_string(i) + "UI.png";
	}

	// スコアアップアニメーションのフラグ
	isScoreUpAnimation_ = false;

	// 前フレームのスコア
	preScore_ = 0;

}

void Player::Update() {

	// 当たり判定用の球の中心を更新
	collisionSphere_.center = transform_.translate;

	// 左右移動
	UpdatePlayerHorizontalMove();

	// プレイヤーの移動制限
	PlayerMoveLimit();

	// プレイヤーの上昇
	if (playerIsMove_) {
		MovePlayerUpward();
	}

	// プレイヤーの速度を一定の値に収める
	ClampPlayerVelocity();

	// プレイヤーの反転処理
	ReverseIfAboveLimit(GAME_END_LINE, END_LINE);

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

	// 弾とトゲの当たり判定
	CollisionBulletThorn();

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

	// カメラの追従オンオフ切り替え
	UpdateCameraSetChange();

	// プレイヤーの点滅
	UpdateFlicker();

	TranslateLerp();

	// モデルに座標情報を反映
	model_->SetTransform(transform_);
	model_->Update();

	// プレイヤーの羽の位置をプレイヤーに合わせる
	playerWing_->SetPosition(transform_.translate);

	// プレイヤーの羽の更新
	playerWing_->Update();

	// クマ
	bear_->SetTranslate(transform_.translate);
	bear_->Update();

	//

	UpdateParticle();

	// オーディオの更新
	AudioUpdate();

	// スコアの比較
	ComparisonScore();

	// スコア加算パーティクル更新
	ScoreParticleAddUpdate();

	// スコア加算パーティクル
	for (auto& digitSprites : spriteAddScoreParticle_) {
		for (auto& sprite : digitSprites) {
			sprite->Update();
		}
	}

}

void Player::Draw(Camera useCamera) {

	// プレイヤー描画
	if (currentFlickFrames_ % 2 == 0) {
		model_->Draw(useCamera);
	}

	// 弾の描画
	for (auto& bullet : bullets_) {
		bullet->Draw(useCamera);
	}

	// プレイヤーの羽の描画
	if (currentFlickFrames_ % 2 == 0) {
		playerWing_->Draw(useCamera);
	}

	// クマ
	if (currentFlickFrames_ % 2 == 0) {
		bear_->Draw(useCamera);
	}

	DrawParticle(useCamera);

	// スコア加算パーティクル
	for (auto& digitSprites : spriteAddScoreParticle_) {
		for (auto& sprite : digitSprites) {
			sprite->Draw();
		}
	}
}

void Player::DrawImgui() {

	PlayerImGui();

	// プレイヤーのステータス
	DrawImGuiJsonStatePlayer();

	armHitParticle4_->DrawImGui("4");
}

void Player::SetBulletGaugeSprites(std::array<BulletGaugeInfo, 5>* gaugeSprites) { bulletGaugeSprites_ = gaugeSprites; }

void Player::MovePlayerUpward() {
	// 上昇
	velocity_.y += acceleration_.y * deltaTime_;
	transform_.translate.y += velocity_.y * deltaTime_;
}

void Player::ClampPlayerVelocity() {
	// 弾の数に応じてプレイヤーの最高速度を変更
	if (bulletGauge_ >= 0 && bulletGauge_ < playerMaxSpeeds_.size()) {
		playerState_.maxSpeed = playerMaxSpeeds_[bulletGauge_];
	}

	velocity_.y = std::clamp(velocity_.y, -playerState_.maxSpeed, playerState_.maxSpeed);
}

void Player::ReverseIfAboveLimit(float minHeight, float maxHeight) {
	// プレイヤーが最高地点に到達したとき
	if (transform_.translate.y >= maxHeight && direction_ == PlayerDirection::UP) {
		// プレイヤーの進行方向を徐々に反対方向に
		acceleration_.y *= -1;

		// プレイヤーの進行状況
		direction_ = PlayerDirection::DOWN;

		// 反転時の無駄な時間を減らす弾の処理
		velocity_.y = 2.0f;

		ramuneOffsetY_ = 1.5f;

		stateChangeParticle_->Play();
		stateChangeTimer_.Start(2.0f, false);

		boost2Particle_->SetOffSet({ 0.0f,2.0f,0.0f });
		boost2Particle_->SetTexture("resources/image/particle/flame.png");
	}

	// プレイヤーが最低地点に到達したとき
	if (transform_.translate.y <= minHeight && direction_ == PlayerDirection::DOWN) {
		if (!isGoal_) {
			goalParticle1_->SetEmitterPosition({ 5.0f, -22.5f, 0.0f });
			goalParticle1_->Play(false);
			goalParticle2_->SetEmitterPosition({ -5.0f, -22.5f, 0.0f });
			goalParticle2_->Play(false);
			// ゴールフラグをたてる
			isGoal_ = true;
		}

		// カメラのオフセット変更
		playerState_.cameraOffset = CAMERA_OFFSET_BOTTOM;

		// SEの解放
		AudioReset();
	}
}

void Player::CameraOffsetChange() {
	// 方向に応じて目標オフセットを決定
	float targetOffset = (direction_ == PlayerDirection::UP) ? CAMERA_OFFSET_TOP : CAMERA_OFFSET_BOTTOM;

	// velocityの符号が反転したかチェック
	bool velocityFlipped = (direction_ == PlayerDirection::UP && velocity_.y > 0.0f) || (direction_ == PlayerDirection::DOWN && velocity_.y < 0.0f);

	// 徐々にオフセットを目標値に近づける
	if (velocityFlipped) {
		playerState_.cameraOffset += velocity_.y * deltaTime_;
	}

	playerState_.cameraOffset = std::clamp(playerState_.cameraOffset, CAMERA_OFFSET_BOTTOM, CAMERA_OFFSET_TOP);
}

void Player::RotateChange() {
	// 目標回転角
	float targetRotationZ = (direction_ == PlayerDirection::UP) ? 0.0f : std::numbers::pi_v<float>;

	// 補間係数
	const float smoothing = 0.1f;

	// 補間処理
	transform_.rotate.z = std::lerp(transform_.rotate.z, targetRotationZ, smoothing);

	if (std::abs(transform_.rotate.z - targetRotationZ) < 0.01f) {
		transform_.rotate.z = targetRotationZ;
	}

	// クマのモデルも同時に回転
	bear_->SetRotate(transform_.rotate);

	// 方向に応じてクマのモデルの配置を変更
	bear_->SetOffsetX((direction_ == PlayerDirection::DOWN) ? -0.2f : 0.2f);
}

void Player::BulletCharge() {
	if (!isCountDownZero_)
		return;

	// ゲージが最大値なら早期リターン
	if (bulletGauge_ >= bulletState_.bulletGaugeMax) {
		return;
	}

	num_++;

	// 2秒に一回ゲージをためる
	if (num_ >= chrageSpeed_) {
		bulletGauge_++;
		(*bulletGaugeSprites_)[bulletGauge_ - 1].isActive = true;
		num_ = 0;

		// SE再生
		gaugeChargeSE_->PlayAudio();

		bulletChargeParticle_->Play(false);
		boost2Particle_->Play(false);
	}
}

void Player::BulletShot() {
	if (bulletGauge_ <= 0 || !isCountDownZero_) {
		return;
	}

	bool isShot = gamePad_->TriggerButton(gamePad_->A) || input_->TriggerKey(DIK_SPACE);

	if (isShot && !stunTimer_.IsActive()) { // スタン中に弾は撃てない
		// 弾の生成
		auto bullet = std::make_unique<Bullet>();
		bullet->Initialize(dxCommon_);

		// 弾が消える際のコールバックを設定
		bullet->SetOnDestroyCallback([this](Vector3 position) {
			// 弾が消えた位置でパーティクルを再生
			bulletDieParticle_->SetEmitterPosition(position);
			bulletDieParticle_->Play(false);
			});

		// プレイヤーの向いてる方向に応じて弾の進む向きも決まる
		if (direction_ == PlayerDirection::UP) {
			bullet->Spawn(transform_.translate, bulletState_.maxSpeed);
		} else {
			bullet->Spawn(transform_.translate, -bulletState_.maxSpeed);
		}

		bullets_.push_back(std::move(bullet));

		// ゲージを減らす
		--bulletGauge_;

		// 描画用のフラグを下ろす
		(*bulletGaugeSprites_)[bulletGauge_].isActive = false;

		// ショットカウント加算
		shotCount_++;

		// タイマーのリセット
		num_ = 0;

		// SEの再生
		shotSE_->PlayAudio();

		//
		bulletShotParticle_->Play(false);
	}
}

void Player::BulletUpdate() {
	// 弾の更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// 生きている弾にBulletMoveParticleをセット
	int activeParticleIndex = 0;

	// 全てのBulletMoveParticleを一旦停止
	for (int i = 0; i < 5; ++i) {
		bulletMoveParticles_[i]->Stop();
	}

	// 生きている弾の位置にBulletMoveParticleのEmitterPosをセット
	for (auto& bullet : bullets_) {
		if (bullet->GetIsAlive() && activeParticleIndex < 5) {
			bulletMoveParticles_[activeParticleIndex]->SetEmitterPosition(bullet->GetTransform().translate);
			if (!bulletMoveParticles_[activeParticleIndex]->IsPlaying()) {
				bulletMoveParticles_[activeParticleIndex]->Play();
			}
			activeParticleIndex++;
		}
	}

	float playerPosY = transform_.translate.y;

	// 弾の削除
	if (!bullets_.empty()) {
		bullets_.erase(
			std::remove_if(
				bullets_.begin(), bullets_.end(),
				[playerPosY](const std::unique_ptr<Bullet>& bullet) { // playerPosYをキャプチャ
					float y = bullet->GetTransform().translate.y;
					bool shouldDelete = y > playerPosY + 14.0f || y < playerPosY - 14.0f; // 画面外に出たら削除

					if (shouldDelete) {
						// 削除前にDestroy()を呼び出してパーティクルを再生
						bullet->Destroy();
					}

					return shouldDelete;
				}),
			bullets_.end());
	}
}

void Player::PlayerImGui() {

	// プレイヤーのImGui
	ImGui::Begin("Player Control");

	ShowLabeledVector3("Translate", &transform_.translate.x);

	ImGui::Checkbox("Player Move", &playerIsMove_);

	ImGui::End();

	ImGui::Begin("Player Audio");

	ImGui::DragFloat("shotSE", &shotSE_BaseVolume_, 0.01f);
	ImGui::DragFloat("damage", &playerDamageSE_BaseVolume_, 0.01f);
	ImGui::DragFloat("getItem", &getItemSE_BaseVolume_, 0.01f);
	ImGui::DragFloat("gaugeCharge", &gaugeChargeSE_BaseVolume_, 0.01f);
	ImGui::DragFloat("attackEnemy", &attackEnemySE_BaseVolume_, 0.01f);
	ImGui::DragFloat("destroyEnemy", &DestroyEnemySE_BaseVolume_, 0.01f);

	ImGui::End();
}

void Player::DrawImGuiJsonStatePlayer() {
	ImGui::Begin("Player State");

	nlohmann::json jsonState = playerState_;

	// JsonNo中身をImGuiで表示する
	DrawImGuiForJson(jsonState, 0.01f);

	playerState_ = jsonState.get<PlayerState>();

	// --- JSONへ保存ボタン ---
	if (ImGui::Button("Save JSON")) {
		JsonState::Save("Resources/Data/playerState.json", playerState_);
	}

	ImGui::DragFloat3("tra", &transform_.translate.x, 0.01f);
	ImGui::Checkbox("isMove", &playerIsMove_);

	ImGui::End();
}

void Player::SpeedDown(float speedDpwnStrength) {
	// 減速
	velocity_.y += (direction_ == PlayerDirection::UP ? -speedDpwnStrength : speedDpwnStrength);
}

void Player::Stun() {
	// 敵にヒットしたら減速
	if (!stunTimer_.IsActive()) {
		stunTimer_.Start(playerState_.stunDuration, false);

		// 減速
		SpeedDown(playerState_.speedDownStrengthThorn);

		// 回転
		StunRotate();

		// スコア減算
		AddScore(scoreList_.stunAmount);

		// SE再生
		playerDamageSE_->PlayAudio();

		// パーティクル停止
		ramuneParticle_->Stop();
		ramuneWhiteParticle_->Stop();

		stunParticle_->Play(transform_.translate);
	}
}

void Player::StunRemoved() { stunTimer_.Update(); }

void Player::CollisionThorn() {
	for (auto& thorn : thorns_) {
		if (Collision::IsHit(thorn->GetCollisionAABB(), collisionAABB_) && thorn->GetIsAlive()) {
			if (direction_ == PlayerDirection::DOWN) {

				// シェイク用のフラグを立てる
				if (!isShake_) {
					StartCameraShake(ShakeType::AttackEnemy);
				}

				// スコア加算
				AddScoreByDistance(thorn, scoreList_.enemyHitAmount);

				// パーティクル
				
				switch (thorn->GetThornType()) {
				case ThornType::MIN:
					ScoreParticleAdd(scoreList_.enemyHitAmount);	// 等倍
					break;
				case ThornType::MIDDLE:
					ScoreParticleAdd(scoreList_.enemyHitAmount*2);	// 2倍
					break;	
				case ThornType::MAX:
					ScoreParticleAdd(scoreList_.enemyHitAmount*3);	// 3倍
					break;
				default:
					break;
				}

				// ゲームパッドの振動
				gamePad_->SetVibration(0.05f, 0.05f, 0.1f);

				// トゲを非アクティブにする
				thorn->PlayParticle(2);
				thorn->SetIsAlive(false);
				smorkParticle_->SetEmitterPosition(thorn->GetPosition());
				smorkParticle_->Play(false);

				// SE再生
				//DestroyEnemySE_->PlayAudio(SE_Volume);
				// SE再生
				attackEnemySE_->PlayAudio();

				break;
			} else if (direction_ == PlayerDirection::UP) {

				// シェイク用のフラグを立てる
				if (!isShake_ && !isStartCoolDown_ && !stunTimer_.IsActive()) {
					StartCameraShake(ShakeType::HitByEnemy);

					// スタンカウント加算
					stunCount_++;

					// 弾のゲージを減らす
					if (bulletGauge_ >= 0) {
						bulletGauge_ -= 1;

						// ゲージを増加させるためのタイマーをリセット
						num_ = 0;
					}

					// 点滅用のフラグを立てる
					if (!isFlicker_) {
						isFlicker_ = true;
					}

					// ゲームパッドの振動
					gamePad_->SetVibration(0.25f, 0.25f, 0.5f);

					// 弾のゲージをセット
					// SetBulletGauge(-1);

					// 弾のゲージスプライトの表示切替
					(*bulletGaugeSprites_)[bulletGauge_].isActive = false;

					// スタン
					Stun();

					// 羽のクールダウン開始
					isStartCoolDown_ = true;
					break;
				}
			}
		}
	}
}

void Player::CollisionBulletThorn() {
	for (auto& bullet : bullets_) {
		for (auto& thorn : thorns_) {
			if (!thorn->CanUpgradeBullet()) {
				continue; // クールダウン中のトゲはスキップ
			}

			if (Collision::IsHit(bullet->GetCollisionSphere(), thorn->GetCollisionSphere())) {

				// SE再生
				getItemSE_->PlayAudio();

				// パーティクル
				thorn->PlayParticle(1);
				smorkParticle_->SetEmitterPosition(thorn->GetPosition());
				smorkParticle_->Play(false);

				// スコア加算
				AddScore(scoreList_.shotHitAmount);

				// パーティクル
				ScoreParticleAdd(scoreList_.shotHitAmount);

				switch (thorn->GetThornType()) {
				case ThornType::MIN:
					thorn->SetThornType(ThornType::MIDDLE);
					thorn->SetUpgradeCooldownBullet(7); // 10フレームのクールダウン
					break;
				case ThornType::MIDDLE:
					thorn->SetThornType(ThornType::MAX);
					thorn->SetUpgradeCooldownBullet(7);
					break;
				default:
					break;
				}
			}
		}
	}
}

void Player::UpdateCameraShake() {
	if (!isShake_)
		return;

	shakeTimer_ -= deltaTime_;
	playerState_.shakeStrength -= shakeDecayRate_ * deltaTime_;
	playerState_.shakeStrength = std::max(0.0f, playerState_.shakeStrength);

	shakeAmount_.x = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * playerState_.shakeStrength;
	shakeAmount_.y = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * playerState_.shakeStrength;

	if (shakeTimer_ <= 0.0f || playerState_.shakeStrength <= 0.01f) {
		isShake_ = false;
		shakeAmount_ = { 0.0f, 0.0f };
	}
}

void Player::WingStateUpdate() {
	if (direction_ == PlayerDirection::UP) {
		playerWing_->SetIsAlive(true);
	} else if (direction_ == PlayerDirection::DOWN) {
		playerWing_->SetIsAlive(false);
	}
}

void Player::AddScore(float score) { score_ += score; }

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

			// トゲの回転
			thorn->SetIsRotate(true);

			// トゲの揺れ
			thorn->SetIsShaking(true);

			if (dis < kNearThreshold) {
				AddScoreByDistance(thorn, scoreList_.wingHitNearAmount); // 近距離スコア

				// パーティクル
				

				switch (thorn->GetThornType()) {
				case ThornType::MIN:
					ScoreParticleAdd(scoreList_.wingHitNearAmount);
					break;
				case ThornType::MIDDLE:
					ScoreParticleAdd(scoreList_.wingHitNearAmount*2);
					break;
				case ThornType::MAX:
					ScoreParticleAdd(scoreList_.wingHitNearAmount*3);
					break;
				default:
					break;
				}

				thorn->PlayParticle(1);

				armHitParticle4_->SetEmitterPosition({ thorn->GetPosition().x, thorn->GetPosition().y, thorn->GetPosition().z });
				armHitParticle4_->Play(false);
			} else {
				AddScoreByDistance(thorn, scoreList_.wingHitFarAmount); // 遠距離スコア

				// パーティクル

				switch (thorn->GetThornType()) {
				case ThornType::MIN:
					ScoreParticleAdd(scoreList_.wingHitFarAmount);
					break;
				case ThornType::MIDDLE:
					ScoreParticleAdd(scoreList_.wingHitFarAmount * 2);
					break;
				case ThornType::MAX:
					ScoreParticleAdd(scoreList_.wingHitFarAmount * 3);
					break;
				default:
					break;
				}

				thorn->PlayParticle(1);

				armHitParticle1_->SetEmitterPosition({ thorn->GetPosition().x, thorn->GetPosition().y, thorn->GetPosition().z});
				armHitParticle1_->Play(false);
				armHitParticle2_->SetEmitterPosition({ thorn->GetPosition().x, thorn->GetPosition().y, thorn->GetPosition().z});
				armHitParticle2_->Play(false);
				armHitParticle3_->SetEmitterPosition({ thorn->GetPosition().x, thorn->GetPosition().y, thorn->GetPosition().z});
				armHitParticle3_->Play(false);
			}

			thorn->SetUpgradeCooldownWing(10); // 10フレームのクールダウン

			// SE再生
			getItemSE_->PlayAudio();

			break;
		}
	}
}

void Player::SetBulletGauge(int point) {
	bulletGauge_ += point;
	if (bulletGauge_ < 0) {
		bulletGauge_ = 0;
	}
	if (bulletGauge_ > bulletState_.bulletGaugeMax) {
		bulletGauge_ = bulletState_.bulletGaugeMax;
	}
}

void Player::AddScoreByDistance(std::shared_ptr<Thorn>& thorn, float scoreAmount) {
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

void Player::DrawImGuiForJson(nlohmann::json& json, float changeFloat) {
	for (auto& [key, value] : json.items()) {
		if (value.is_number_float()) {
			float val = value.get<float>();
			if (ImGui::DragFloat(key.c_str(), &val, changeFloat))
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
				DrawImGuiForJson(value, changeFloat);
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
	DrawImGuiForJson(jsonState, 0.01f);

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
	if (transform_.translate.x >= moveLimitPosX_) {
		transform_.translate.x = moveLimitPosX_;
	}

	if (transform_.translate.x <= -moveLimitPosX_) {
		transform_.translate.x = -moveLimitPosX_;
	}
}

void Player::UpdateCollisionAABB() {
	Vector3 t = transform_.translate;
	collisionAABB_.max = { t.x - 0.4f, t.y - 0.2f, t.z + 0.4f };
	collisionAABB_.min = { t.x + 0.1f, t.y + 0.2f, t.z - 0.4f };
}

void Player::StunRotate() {
	// 2回転
	transform_.rotate.z += 4.0f * std::numbers::pi_v<float>;
}

void Player::UpdateCameraSetChange() {
	if (transform_.translate.y >= CAMERA_OFFSET_BOTTOM && !isCameraSet_) {
		isCameraSet_ = true;
	}

	if (transform_.translate.y <= GAME_END_LINE && isCameraSet_ && direction_ == PlayerDirection::DOWN) {
		isCameraSet_ = false;
	}
}

void Player::UpdatePlayerHorizontalMove() {
	if (!isCameraSet_)
		return;

	bool isLeftMove = gamePad_->LeftStickX() <= -0.3f || gamePad_->PushButton(gamePad_->LEFT_BOTTON) || input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A);
	bool isRightMove = gamePad_->LeftStickX() >= 0.3f || gamePad_->PushButton(gamePad_->RIGHT_BOTTON) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D);

	const float accelerationFactor = 2.5f; // 反対方向への加速用の定数
	const float attenuationFactor = 1.5f;  // 減速倍率
	const float kMaxSpeed = 10.0f;         // 最高速度

	float accelerationX = 0.0f;

	// 入力方向に応じて加速度を切り替える
	if (isLeftMove) {
		accelerationX = -acceleration_.x;
	}
	if (isRightMove) {
		accelerationX = acceleration_.x;
	}

	// 反対方向への加速する
	if ((accelerationX < 0 && velocity_.x > 0) || (accelerationX > 0 && velocity_.x < 0)) {
		accelerationX *= accelerationFactor;
	}

	// 入力なしなら減速する
	if (!isLeftMove && !isRightMove) {
		if (std::abs(velocity_.x) < acceleration_.x * deltaTime_) {
			velocity_.x = 0.0f;
		} else {
			// xの絶対値にyの符号が付いた値を返す関数
			accelerationX = -std::copysign(acceleration_.x * attenuationFactor, velocity_.x);
		}
	}

	// 最大速度を制限
	velocity_.x = std::clamp(velocity_.x + accelerationX * deltaTime_, -kMaxSpeed, kMaxSpeed);

	// 移動
	transform_.translate.x += velocity_.x * deltaTime_;
}

void Player::AudioUpdate() {
	shotSE_->SetVolume(shotSE_BaseVolume_ * SE_Volume);
	playerDamageSE_->SetVolume(playerDamageSE_BaseVolume_ * SE_Volume);
	DestroyEnemySE_->SetVolume(DestroyEnemySE_BaseVolume_ * SE_Volume);
	gaugeChargeSE_->SetVolume(gaugeChargeSE_BaseVolume_ * SE_Volume);
	getItemSE_->SetVolume(getItemSE_BaseVolume_ * SE_Volume);
	attackEnemySE_->SetVolume(attackEnemySE_BaseVolume_ * SE_Volume);

	shotSE_->Update();
	playerDamageSE_->Update();
	DestroyEnemySE_->Update();
	gaugeChargeSE_->Update();
	getItemSE_->Update();
	attackEnemySE_->Update();
}

void Player::DrawImGuiJsonStateScore() {
	ImGui::Begin("Score List");

	nlohmann::json jsonState = scoreList_;

	// JsonNo中身をImGuiで表示する
	DrawImGuiForJson(jsonState, 1.0f);

	scoreList_ = jsonState.get<ScoreList>();

	// --- JSONへ保存ボタン ---
	if (ImGui::Button("Save JSON")) {
		JsonState::Save("Resources/Data/scoreList.json", scoreList_);
	}

	ImGui::End();
}

void Player::InitParticle() {
	//
	ramuneParticle_->Initialize(dxCommon_, 2);
	ramuneParticle_->LoadJson("ramuneGame");

	ramuneWhiteParticle_->Initialize(dxCommon_, 2);
	ramuneWhiteParticle_->LoadJson("ramuneWhiteGame");

	kasokuParticle_->Initialize(dxCommon_);
	kasokuParticle_->LoadJson("kasoku");
	kasokuParticle_->Play();

	smorkParticle_->Initialize(dxCommon_);
	smorkParticle_->LoadJson("enemydie");
	smorkParticle_->Stop();

	bulletChargeParticle_->Initialize(dxCommon_);
	bulletChargeParticle_->LoadJson("shotCharge");

	bulletShotParticle_->Initialize(dxCommon_);
	bulletShotParticle_->LoadJson("bulletShot");

	bulletDieParticle_->Initialize(dxCommon_);
	bulletDieParticle_->LoadJson("bulletDie");

	ramuneOffsetY_ = -1.5f;

	stunParticle_->Initialize(dxCommon_);

	stateChangeParticle_->Initialize(dxCommon_);
	stateChangeParticle_->LoadJson("stateChange");
	stateChangeParticle_->Stop();

	fallParticle_->Initialize(dxCommon_, 4);
	fallParticle_->LoadJson("fall");
	fallParticle_->Stop();

	stateChangeTimer_.Reset();

	armHitParticle1_->Initialize(dxCommon_);
	armHitParticle1_->LoadJson("armHit1");
	armHitParticle1_->Stop();
	armHitParticle2_->Initialize(dxCommon_);
	armHitParticle2_->LoadJson("armHit2");
	armHitParticle2_->Stop();
	armHitParticle3_->Initialize(dxCommon_);
	armHitParticle3_->LoadJson("armHit3");
	armHitParticle3_->Stop();
	armHitParticle4_->Initialize(dxCommon_);
	armHitParticle4_->LoadJson("armHit4");
	armHitParticle4_->Stop();

	goalParticle1_->Initialize(dxCommon_);
	goalParticle1_->LoadJson("goal");
	goalParticle1_->Stop();

	goalParticle2_->Initialize(dxCommon_);
	goalParticle2_->LoadJson("goal");
	goalParticle2_->Stop();

	getScoreParticle_->Initialize(dxCommon_);
	getScoreParticle_->LoadJson("getScore");

	for (int i = 0; i < 5; ++i) {
		bulletMoveParticles_[i] = make_unique<Particles>();
		bulletMoveParticles_[i]->Initialize(dxCommon_);
		bulletMoveParticles_[i]->LoadJson("bulletMove");
	}

	boostParticle_->Initialize(dxCommon_);
	boostParticle_->LoadJson("boost1");	
	boostParticle_->SetOffSet({ 0.0f, -1.0f, 0.0f });
	boostParticle_->Stop();

	boost2Particle_->Initialize(dxCommon_);
	boost2Particle_->LoadJson("boost2");
	boost2Particle_->SetOffSet({ 0.0f, -2.0f, 0.0f });
	boost2Particle_->Stop();
}

void Player::UpdateParticle() {

	if (isCountDownZero_ && !boostTimer_.IsFinished() && !boostTimer_.IsActive()) {
		boostTimer_.Start(0.5f, false);
		boostParticle_->Play();
	}

	if (boostTimer_.IsFinished()) {
		boostParticle_->Stop();
	}

	boostTimer_.Update();

	if (stunTimer_.IsFinished()) {
		ramuneParticle_->Play();
		ramuneWhiteParticle_->Play();
	}

	ramuneParticle_->SetOffSet({ 0.0f, ramuneOffsetY_, 0.0f });
	ramuneParticle_->SetEmitterPosition(transform_.translate);
	ramuneParticle_->Update();

	ramuneWhiteParticle_->SetOffSet({ 0.0f, ramuneOffsetY_, 0.0f });
	ramuneWhiteParticle_->SetEmitterPosition(transform_.translate);
	ramuneWhiteParticle_->Update();

	if (direction_ == PlayerDirection::UP) {
		kasokuOffsetY_ = 12.0f;
		if (velocity_.y >= 12.0f && velocity_.y <= 14.0f) {
			kasokuParticle_->SetSpawnTime(0.1f);
		} else if (velocity_.y >= 14.0f && velocity_.y <= 16.0f) {
			kasokuParticle_->SetSpawnTime(0.075f);
		} else if (velocity_.y >= 16.0f && velocity_.y <= 18.0f) {
			kasokuParticle_->SetSpawnTime(0.05f);
		} else if (velocity_.y >= 18.0f && velocity_.y <= 20.0f) {
			kasokuParticle_->SetSpawnTime(0.025f);
		} else {
			kasokuParticle_->SetSpawnTime(99.0f);
			kasokuParticle_->SetEmitVelocity({ 0.0f, -10.0f, 0.0f });
		}

		bulletChargeParticle_->SetOffSet({ 0.0f, 1.65f, -0.75f });
	} else if (direction_ == PlayerDirection::DOWN) {
		kasokuOffsetY_ = -12.0f;
		if (velocity_.y >= -14.0f && velocity_.y <= -12.0f) {
			kasokuParticle_->SetSpawnTime(0.1f);
		} else if (velocity_.y >= -16.0f && velocity_.y <= -14.0f) {
			kasokuParticle_->SetSpawnTime(0.075f);
		} else if (velocity_.y >= -18.0f && velocity_.y <= -16.0f) {
			kasokuParticle_->SetSpawnTime(0.05f);
		} else if (velocity_.y >= -20.0f && velocity_.y <= -18.0f) {
			kasokuParticle_->SetSpawnTime(0.025f);
		} else {
			kasokuParticle_->SetSpawnTime(99.0f);
			kasokuParticle_->SetEmitVelocity({ 0.0f, 10.0f, 0.0f });
		}

		bulletChargeParticle_->SetOffSet({ 0.0f, -1.65f, -0.75f });
	}

	kasokuParticle_->SetOffSet({ 0.0f, kasokuOffsetY_, 0.0f });
	kasokuParticle_->SetEmitterPosition(transform_.translate);
	kasokuParticle_->Update();

	smorkParticle_->Update();

	bulletChargeParticle_->SetEmitterPosition(transform_.translate);
	bulletChargeParticle_->Update();

	stunParticle_->Update();

	if (direction_ == PlayerDirection::UP) {
		bulletShotParticle_->SetOffSet({ 0.0f, 0.75f, 0.0f });
		bulletShotParticle_->SetEmitVelocityY(15.0f);
	} else if (direction_ == PlayerDirection::DOWN) {
		bulletShotParticle_->SetOffSet({ 0.0f, -0.75f, 0.0f });
		bulletShotParticle_->SetEmitVelocityY(-15.0f);
	}
	bulletShotParticle_->SetEmitterPosition(transform_.translate);
	bulletShotParticle_->Update();

	bulletDieParticle_->SetOffSet({ 0.0f,0.0f,-5.0f });
	bulletDieParticle_->Update();

	stateChangeParticle_->SetEmitterPosition(transform_.translate);
	stateChangeParticle_->Update();

	fallParticle_->SetEmitterPosition(transform_.translate);
	fallParticle_->SetOffSet({ 0.0, -1.25f, 0.0f });
	fallParticle_->Update();

	if (stateChangeTimer_.IsFinished()) {
		stateChangeParticle_->Stop();
		fallParticle_->Play();
	}
	stateChangeTimer_.Update();

	armHitParticle1_->Update();
	armHitParticle2_->Update();
	armHitParticle3_->Update();
	armHitParticle4_->Update();

	goalParticle1_->Update();
	goalParticle2_->Update();

	for (auto& thorn : thorns_) {
		for (int i = 0; i < 5; i++) {
			if (thorn->GetLifeTimerFinish(i)) {
				getScoreParticle_->SetEmitterPosition(thorn->GetTranslate(i));
				getScoreParticle_->Play(false);
			}

			if (thorn->GetLifeTimerActive(i)) {
				Vector3 thornPos = thorn->GetTranslate(i);
				float playerY = transform_.translate.y;

				switch (direction_) {
				case PlayerDirection::UP:
					if (thornPos.y < playerY - 3.75f) {
						getScoreParticle_->SetEmitterPosition(thornPos);
						getScoreParticle_->SetStartColor(thorn->GetColor(i));
						getScoreParticle_->Play(false);
						thorn->ParticleReset(i);
					}
					break;
				case PlayerDirection::DOWN:
					if (thornPos.y < playerY - 13.75f) {
						getScoreParticle_->SetEmitterPosition(thornPos);
						getScoreParticle_->SetEmitterPosition(thornPos);
						getScoreParticle_->SetStartColor(thorn->GetColor(i));
						getScoreParticle_->Play(false);
						thorn->ParticleReset(i);
					}
					break;
				}
			}
		}
	}

	if (direction_ == PlayerDirection::DOWN) {
		getScoreParticle_->SetEmitVelocityY(3.0f);
	}
	getScoreParticle_->Update();

	// BulletMoveParticleの配列を更新
	for (int i = 0; i < 5; ++i) {
		bulletMoveParticles_[i]->Update();
	}

	boostParticle_->SetEmitterPosition(transform_.translate);
	boost2Particle_->SetEmitterPosition(transform_.translate);

	boostParticle_->Update();
	boost2Particle_->Update();
}

void Player::DrawParticle(Camera useCamera) {
	ramuneParticle_->Draw(useCamera);
	ramuneWhiteParticle_->Draw(useCamera);
	kasokuParticle_->Draw(useCamera);
	smorkParticle_->Draw(useCamera);
	bulletChargeParticle_->Draw(useCamera);
	stunParticle_->Draw(useCamera);
	bulletShotParticle_->Draw(useCamera);
	bulletDieParticle_->Draw(useCamera);

	// BulletMoveParticleの配列を描画
	for (int i = 0; i < 5; ++i) {
		bulletMoveParticles_[i]->Draw(useCamera);
	}

	stateChangeParticle_->Draw(useCamera);
	fallParticle_->Draw(useCamera);
	armHitParticle1_->Draw(useCamera);
	armHitParticle2_->Draw(useCamera);
	armHitParticle3_->Draw(useCamera);
	armHitParticle4_->Draw(useCamera);
	goalParticle1_->Draw(useCamera);
	goalParticle2_->Draw(useCamera);
	getScoreParticle_->Draw(useCamera);

	boostParticle_->Draw(useCamera);
	boost2Particle_->Draw(useCamera);
}

void Player::UpdateFlicker() {
	if (isFlicker_) {
		currentFlickFrames_++;

		// 60フレーム経過したらタイマーを0に初期化
		if (currentFlickFrames_ >= kMaxFrameFlick) {
			currentFlickFrames_ = 0;
			isFlicker_ = false;
		}
	}
}

void Player::StartCameraShake(ShakeType type) {
	auto it = kShakePresets.find(type);
	if (it != kShakePresets.end()) {
		const auto& params = it->second;
		playerState_.shakeStrength = params.strength;
		shakeTimer_ = params.duration;
		shakeDecayRate_ = params.decayRate;
		isShake_ = true;
	}
}

void Player::ComparisonScore() {
	if (score_ > preScore_) {
		// アニメーションフラグを立てる
		isScoreUpAnimation_ = true;

		// スコアを記録
		preScore_ = score_;
	}
}

void Player::ScoreParticleAdd(float score) {
	// スコアを整数に変換
	int displayScore = static_cast<int>(score);

	// 分解
	std::vector<int> digits;
	if (displayScore == 0) {
		digits.push_back(0);
	} else {
		while (displayScore > 0) {
			digits.push_back(displayScore % 10);
			displayScore /= 10;
		}
	}
	std::reverse(digits.begin(), digits.end());

	// 新しい加算用スプライト群を作成
	std::vector<std::unique_ptr<Sprite>> digitSprites;

	// 基準位置を範囲内のランダムな値で決める
	constexpr float MIN_POS_X = 100.0f;
	constexpr float MAX_POS_X = 200.0f;

	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<float> distr(MIN_POS_X, MAX_POS_X);

	Vector2 basePos = { distr(eng), 350.0f };
	float digitSpacing = 32.0f;

	// 桁ごとにスプライト生成
	for (size_t i = 0; i < digits.size(); ++i) {
		int digit = digits[i];

		auto sprite = std::make_unique<Sprite>();
		sprite->Initialize(dxCommon_, "resources/image/UI/Number0UI.png");
		sprite->SetTexture(spriteNumCollection_[digit]);
		sprite->SetScale({ 0.25f, 0.25f });

		if (score == 5) {
			sprite->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f });
		} else if (score == 100) {
			sprite->SetColor({ 0.5490f, 0.9098f, 0.4392f, 1.0f });
		} else if (score == 30) {
			sprite->SetColor({ 0.6275f, 0.9686f, 0.9608f, 1.0f });
		} else if (score == 20) {
			sprite->SetColor({ 0.8549f, 0.6392f, 0.9451f, 1.0f });
		} else if (score == -20) {
			sprite->SetColor({ 0.9373f, 0.4275f, 0.4275f, 1.0f });
		}

		Vector2 digitPos = basePos;
		digitPos.x += static_cast<float>(i) * digitSpacing;
		sprite->SetPosition(digitPos);

		digitSprites.push_back(std::move(sprite));
	}

	spriteAddScoreParticle_.push_back(std::move(digitSprites));
}

void Player::ScoreParticleAddUpdate() {
	for (auto& digitSprites : spriteAddScoreParticle_) {
		for (auto& sprite : digitSprites) {
			constexpr float MIN_SPEED_X = -1.0f;
			constexpr float MAX_SPEED_X = 1.0f;

			std::random_device rd;
			std::mt19937 eng(rd());
			std::uniform_real_distribution<float> distr(MIN_SPEED_X, MAX_SPEED_X);

			sprite->GetColor().w -= 0.05f;
			sprite->GetSize().x -= 5.0f;
			sprite->GetSize().y -= 5.0f;
			sprite->GetPosition().y -= 5.0f;
			sprite->GetPosition().x += distr(eng);
		}

		// 透明度が0以下のスプライトを削除
		digitSprites.erase(std::remove_if(digitSprites.begin(), digitSprites.end(), [](const std::unique_ptr<Sprite>& sprite) { return sprite->GetColor().w <= 0.0f; }), digitSprites.end());
	}

	// 空になったスコアグループも削除
	spriteAddScoreParticle_.erase(
		std::remove_if(spriteAddScoreParticle_.begin(), spriteAddScoreParticle_.end(), [](const std::vector<std::unique_ptr<Sprite>>& group) { return group.empty(); }), spriteAddScoreParticle_.end());
}

void Player::AudioReset() {
	shotSE_->Reset();
	playerDamageSE_->Reset();
	DestroyEnemySE_->Reset();
	gaugeChargeSE_->Reset();
	getItemSE_->Reset();
	attackEnemySE_->Reset();
}

void Player::TranslateLerp() {
	// プレイヤーのY座標を取得
	float currentY = transform_.translate.y;
	
	// START_LINE (-10.0f) が 0.0f、END_LINE (440.0f) が 1.0f になるように線形補間
	// currentYがSTART_LINE以下なら0.0f、END_LINE以上なら1.0fにクランプ
	float t = (currentY - START_LINE) / (END_LINE - START_LINE);
	
	// 0.0f～1.0fの範囲にクランプ
	t = std::clamp(t, 0.0f, 1.0f);
	
	lerp_ = Easing::LerpVector3({ 0.212f, 0.722f, 1.000f }, { 0.0f,0.0f,0.0f }, t);

	// 必要に応じて、この値tを他の処理で使用する
	// 例: プレイヤーの進行度として保存したい場合
	// playerProgress_ = t; // メンバ変数があれば保存
}