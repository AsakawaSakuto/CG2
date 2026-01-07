#include "player.h"
#include "GameObject/EnemyManager/EnemyManager.h"
#include "Map/Map3D.h"
#include "Map/TreeManager/TreeManager.h"
#include "Utility/Random/Random.h"
#include <cmath>
#include <algorithm>
#include <limits>

void Player::PostFrameCleanup() {
	weaponManager_->PostFrameCleanup();
}

void Player::Initialize() {
	// デフォルト値で初期化
	Initialize(PlayerName::PowerMan, WeaponName::FireBall);
}

void Player::Initialize(PlayerName playerName, WeaponName weaponName) {
	playerName_ = playerName;

	switch (playerName_)
	{
	case PlayerName::PowerMan:
		status_.damageRate = 1.2f; // ダメージ倍率アップ
		break;
	case PlayerName::TankMan:
		status_.maxHP = 200;      // 最大体力アップ
		break;
	case PlayerName::JumpMan:
		status_.jumpCanCount = 2; // ジャンプ回数アップ
		break;
	case PlayerName::SpeedMan:
		status_.moveSpeed = 10.0f; // 移動速度アップ
		break;
	default:
		break;
	}

	status_.currentHP = status_.maxHP;
	status_.currentExp = 0;
	status_.level = 1;

	transform_.SetAllScale(1.0);
	transform_.translate = { 10.0f,100.0f,10.0f };

	// PlayerModelControllerの初期化
	// アニメーションをロード（gltfファイル内のアニメーションインデックスを指定）
	Animation idle =      LoadAnimationFile("Player/Animation/idle.gltf");
	Animation crouIdle =  LoadAnimationFile("Player/Animation/crouIdle.gltf");
	Animation walk =      LoadAnimationFile("Player/Animation/walk.gltf");
	Animation crouching = LoadAnimationFile("Player/Animation/Crouching.gltf");
	Animation jump =      LoadAnimationFile("Player/Animation/jump.gltf");
	Animation landing =   LoadAnimationFile("Player/Animation/landing.gltf");
	Animation die =       LoadAnimationFile("Player/Animation/die.gltf");

	std::map<PlayerMotion, Animation> animationMap;
	animationMap[PlayerMotion::Idle] =      idle;
	animationMap[PlayerMotion::CrouIdle] =  crouIdle;
	animationMap[PlayerMotion::Walk] =      walk;
	animationMap[PlayerMotion::Crouching] = crouching;
	animationMap[PlayerMotion::Jump] =      jump;
	animationMap[PlayerMotion::Landing] =   landing;
	animationMap[PlayerMotion::Die] =       die;

	// AnimationControllerを作成してから初期化
	model_ = std::make_unique<AnimationController>();
	model_->Initialize(animationMap);
	model_->SetMotion(PlayerMotion::Idle, 0.0f, true);
	model_->SetColor(playerColors[static_cast<int>(playerName_)]);

	moveParticle_->Initialize("playerMove");
	landingParticle_->Initialize("playerLanding");

	weaponManager_->Initialize();
	
	// 選択した武器を最初に装備
	weaponManager_->EquipWeapon(weaponName);

	// AABBの初期化（新仕様: center + min/maxのローカルオフセット）
	// center は transform_.translate で毎フレーム更新されるため、ここでは初期化不要
	// min/max はローカル空間でのオフセットとして設定
	mapCollosion_.center = { 0.0f, 0.0f, 0.0f }; // 初期値（Update()で更新される）
	mapCollosion_.min = { -0.5f, 0.0f, -0.5f };  // centerからのローカルオフセット
	mapCollosion_.max = {  0.5f, 0.25f,  0.5f };  // centerからのローカルオフセット

	upgradeManager_ = std::make_unique<UpgradeManager>();
	upgradeManager_->Initialize();
	// UpgradeManagerにWeaponManagerを設定
	upgradeManager_->SetWeaponManager(weaponManager_.get());

	healingTimer_.Start(6.0f, true);

	isDie_ = false;
}

void Player::Update() {

	if (!upgradeManager_->IsUpgradeSelect() && isAlive_) {

		if (status_.currentHP <= 0 && currentMotion_ != PlayerMotion::Die) {
			MyAudio::Play(SE_List::PlayerDie);
			model_->SetMotion(PlayerMotion::Die, 0.0f, false);
			currentMotion_ = PlayerMotion::Die;
			isAlive_ = false;
		}

		if (!isAlive_) {
			return;
		}

		if (transform_.translate.y <= -10.0f) {
			transform_.translate = startPos_;
		}

		if (MyInput::Push(Action::Y)) {
			comeBackTimer_ += deltaTime_;
			MyInput::SetVibration(1.0f, 1.0f, 0.1f);

			if (comeBackTimer_ >= 5.0f) {
				comeBackTimer_ = 0.0f;
				transform_.translate = startPos_;
			}
		} else {
			comeBackTimer_ = 0.0f;
		}

		if (healingTimer_.IsFinished()) {
			status_.currentHP++;
		}
		status_.currentHP = std::clamp(status_.currentHP, 0, status_.maxHP);
		healingTimer_.Update();

		Move();
		Jump();
		SlideOnSlope();  // しゃがみ中のスロープ滑り処理

		// AABBの中心をプレイヤーの位置に設定
		mapCollosion_.center = transform_.translate;

		// マップとの衝突解決を実行
		if (map_) {
			ResolveMapCollision();
		}

		// 木との衝突解決を実行（XZ軸のみ）
		if (treeManager_) {
			treeManager_->ResolvePlayerCollision(transform_.translate, mapCollosion_);
			// 衝突解決後、AABBの中心も更新
			mapCollosion_.center = transform_.translate;
		}

		MyDebugLine::AddShape(mapCollosion_, { 1.0f,0.0f,0.0f,1.0f });

		directionToEnemy_ = GetDirectionToEnemy();

		expGetRangeTransform_.translate = transform_.translate;
		expGetRangeTransform_.scale = { 7.0f, 1.0f, 7.0f };

		// 影のY座標を地面の高さに設定
		if (map_) {
			float groundY = GetGroundHeight();
			model_->SetShadowGroundY(groundY);
		}

		moveParticle_->SetOffSet({ 0.0f, 0.1f, 0.0f });
		moveParticle_->Update();
		landingParticle_->SetOffSet({ 0.0f, 0.1f, 0.0f });
		landingParticle_->Update();

		// 通常の最も近い敵への方向とランダムターゲット方向の両方を設定
		Vector3 closestEnemyDirection = GetDirectionToClosestEnemy();
		weaponManager_->SetDirectionToEnemy(closestEnemyDirection);
		weaponManager_->SetRandomDirectionToEnemy(directionToEnemy_);
		weaponManager_->SetPlayerPosition(transform_.translate);
		weaponManager_->Update();

		sphereCollision_.center = transform_.translate;
		sphereCollision_.radius = collisionRadius_;

		expItemStateChangeCollision_.center = transform_.translate;
		expItemStateChangeCollision_.radius = expItemStateChangeRadius_;

		// 無敵時間タイマーの更新
		invincibilityTimer_.Update();

		// 無敵時間中の点滅処理
		if (invincibilityTimer_.IsActive()) {
			// 点滅タイマーが動作していない場合は開始
			if (!blinkTimer_.IsActive()) {
				blinkTimer_.Start(0.1f, true); // 0.1秒ごとに点滅
				isVisible_ = true;
			}

			blinkTimer_.Update();

			// タイマーが完了したら表示/非表示を切り替え
			if (blinkTimer_.IsFinished()) {
				isVisible_ = !isVisible_;
			}
		} else {
			// 無敵時間が終了したら常に表示
			blinkTimer_.Stop();
			isVisible_ = true;
		}
	}

	upgradeManager_->Update();

	if (!isDie_) {
		if (currentMotion_ == PlayerMotion::Die && model_->GetAnimationProgress() >= 0.95f) {
			isDie_ = true;
		}

		model_->Update(1.0f / 60.0f, transform_);
	}

	/*if (MyInput::TriggerKey(DIK_0)) {
		upgradeManager_->Upgrade();
	}
	if (MyInput::TriggerKey(DIK_9)) {
		status_.currentHP -= 10;
	}*/


	MyDebugLine::AddShape(sphereCollision_);
	Circle expCircle = {};
	expCircle.center = { transform_.translate.x ,transform_.translate.y + 0.1f ,transform_.translate.z };
	expCircle.radius = expItemStateChangeRadius_;
	expCircle.normal = { 0.0f,1.0f,0.0f };
	MyDebugLine::AddShape(expCircle);
}

void Player::Draw(Camera camera) {
	// カメラを保存（移動計算で使用）
	camera_ = camera;

	// 無敵時間中で非表示の場合はモデルを描画しない
	if (isVisible_) {
		model_->Draw(camera);
	}

	//expItemGetRange_->Draw(camera, expGetRangeTransform_);

	moveParticle_->Draw(camera);
	landingParticle_->Draw(camera);

	weaponManager_->Draw(camera);

	upgradeManager_->Draw();
}

void Player::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Player Status");
	
	// 基本ステータス表示
	ImGui::Text("Level: %d", status_.level);
	ImGui::Text("HP: %d / %d", status_.currentHP, status_.maxHP);
	ImGui::Text("Exp: %d / %d", status_.currentExp, status_.expToNextLevel);
	ImGui::Text("Money: %d", status_.nowMoney);
	ImGui::Text("Total Kills: %d", status_.killEnemyCount);
	
	ImGui::Separator();
	
	// 武器別キルカウント表示
	ImGui::Text("Weapon Kill Counts:");
	
	// 各武器のキルカウントを表示
	const char* weaponNames[] = {
		"None", "FireBall", "Laser", "Runa", "Axe", 
		"Boomerang", "Dice", "Toxic", "Area", "Gun"
	};
	
	for (int i = 0; i < static_cast<int>(WeaponName::Count); ++i) {
		WeaponName weaponName = static_cast<WeaponName>(i);
		int killCount = GetWeaponKillCount(weaponName);
		
		// キルカウントが0より大きい武器のみ表示
		if (killCount > 0) {
			ImGui::Text("  %s: %d kills", weaponNames[i], killCount);
		}
		
		// 現在装備している武器かチェック
		if (HasWeapon(weaponName) && weaponName != WeaponName::None) {
			ImGui::SameLine();
			ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, " (Equipped)");
		}
	}
	
	ImGui::Separator();
	ImGui::Text("Total Weapon Kills: %d", GetTotalWeaponKillCount());
	
	ImGui::End();
#endif
	//landingParticle_->DrawImGui("move Particle");

	upgradeManager_->DrawImGui();
}

void Player::Move() {

	// KeyConfigを使って移動入力を取得
	InputManager::Vector2D moveInput = { 0.0f, 0.0f };
	
	// GamePadが接続されている場合、スティック入力を取得
	if (MyInput::UseGamePad()) {
		moveInput = MyInput::GetVector2D(Action::MOVE_STICK);
		
		// スティック入力がほぼゼロの場合、キーボード入力も確認
		if (std::abs(moveInput.x) < 0.01f && std::abs(moveInput.y) < 0.01f) {
			// 各方向の入力を確認
			if (MyInput::Push(Action::MOVE_UP)) {
				moveInput.y += 1.0f;
			}
			if (MyInput::Push(Action::MOVE_DOWN)) {
				moveInput.y -= 1.0f;
			}
			if (MyInput::Push(Action::MOVE_RIGHT)) {
				moveInput.x += 1.0f;
			}
			if (MyInput::Push(Action::MOVE_LEFT)) {
				moveInput.x -= 1.0f;
			}
		}
	} else {
		// GamePadが接続されていない場合、キーボード入力を確認
		// 各方向の入力を確認
		if (MyInput::Push(Action::MOVE_UP)) {
			moveInput.y += 1.0f;
		}
		if (MyInput::Push(Action::MOVE_DOWN)) {
			moveInput.y -= 1.0f;
		}
		if (MyInput::Push(Action::MOVE_RIGHT)) {
			moveInput.x += 1.0f;
		}
		if (MyInput::Push(Action::MOVE_LEFT)) {
			moveInput.x -= 1.0f;
		}
	}
	
	// スティックの入力があるかチェック
	if (std::abs(moveInput.x) > 0.01f || std::abs(moveInput.y) > 0.01f) {
		// カメラから移動方向ベクトルを計算
		Vector3 moveDirection = CalculateCameraMoveDirection(moveInput.x * -1.0f, moveInput.y * -1.0f);

		// 移動量を計算
		Vector3 movement = {
			moveDirection.x * status_.moveSpeed * deltaTime_,
			0.0f, // Y軸移動は制限
			moveDirection.z * status_.moveSpeed * deltaTime_
		};

		// プレイヤーの位置を更新
		transform_.translate.x += movement.x;
		transform_.translate.z += movement.z;

		// プレイヤーの向きを移動方向に合わせる
		if (std::abs(moveDirection.x) > 0.01f || std::abs(moveDirection.z) > 0.01f) {
			float targetYaw = std::atan2(moveDirection.x, moveDirection.z);
			transform_.rotate.y = targetYaw;
		}
		
		if (!moveParticleTimer_.IsActive()) {
			moveParticleTimer_.Start(0.2f, true);
		} else {
			moveParticleTimer_.Update();
		}
		if (moveParticleTimer_.IsFinished()) {
			moveParticle_->Play(transform_.translate, false);
		}

		// 移動中のアニメーション判定
		if (MyInput::Push(Action::CROUCHING)) {
			// しゃがみ歩き
			if (currentMotion_ != PlayerMotion::Crouching) {
				model_->SetMotion(PlayerMotion::Crouching, 0.1f, true);
				currentMotion_ = PlayerMotion::Crouching;
			}
		} else {
			// 通常歩き
			if (currentMotion_ != PlayerMotion::Walk) {
				model_->SetMotion(PlayerMotion::Walk, 0.1f, true);
				currentMotion_ = PlayerMotion::Walk;
			}
		}
	} else {
		// 移動入力がない場合
		moveParticle_->Stop();
		moveParticleTimer_.Stop();

		// 待機状態に遷移
		if (MyInput::Push(Action::CROUCHING)) {
			if (currentMotion_ != PlayerMotion::CrouIdle) {
				model_->SetMotion(PlayerMotion::CrouIdle, 0.1f, true);
				currentMotion_ = PlayerMotion::CrouIdle;
			}
		} else {
			if (currentMotion_ != PlayerMotion::Idle) {
				model_->SetMotion(PlayerMotion::Idle, 0.1f, true);
				currentMotion_ = PlayerMotion::Idle;
			}
		}
	}

	transform_.translate.x = std::clamp(transform_.translate.x, -7.0f, 217.0f);
	transform_.translate.z = std::clamp(transform_.translate.z, -7.0f, 217.0f);
}

Vector3 Player::CalculateCameraMoveDirection(float stickX, float stickY) {
	
	// カメラの水平角度を取得（Y軸周りの回転）
	float cameraHorizontalAngle = camera_.GetHorizontalAngle();
	
	// カメラの水平角度から前方向と右方向を計算
	// カメラの水平角度が0の時、カメラはZ軸正の方向を向いている
	// 左スティックを上に倒す（stickY > 0）→ カメラの向いている方向に移動
	
	// 前方向（カメラが向いている方向）
	Vector3 forward = {
		std::sin(cameraHorizontalAngle),   // X成分
		0.0f,                               // Y成分は無視（水平移動のみ）
		std::cos(cameraHorizontalAngle)    // Z成分
	};
	
	// 右方向（前方向を90度時計回りに回転）
	Vector3 right = {
		std::cos(cameraHorizontalAngle),   // X成分
		0.0f,                               // Y成分は無視
		-std::sin(cameraHorizontalAngle)   // Z成分
	};
	
	// スティック入力に応じて移動方向を計算
	// stickY > 0: 上に倒す → カメラの向いている方向（前方向）に移動
	// stickY < 0: 下に倒す → カメラと反対方向（後方向）に移動  
	// stickX > 0: 右に倒す → カメラから見て右方向に移動
	// stickX < 0: 左に倒す → カメラから見て左方向に移動
	Vector3 moveDirection = {
		forward.x * stickY + right.x * stickX,
		0.0f,
		forward.z * stickY + right.z * stickX
	};
	
	return moveDirection.Normalized();
}

void Player::Jump() {
	// 前フレームの地面接触状態を保存
	wasGrounded_ = isGrounded_;

	// マップとの地面判定を使用
	if (map_) {
		isGrounded_ = IsGroundedOnMap();
	} else {
		// マップが無い場合は従来の判定を使用
		if (transform_.translate.y <= groundLevel_) {
			transform_.translate.y = groundLevel_;
			status_.velocity_Y = 0.0f;
			isGrounded_ = true;
		} else {
			isGrounded_ = false;
		}
	}

	// スロープ上にいる場合はY座標を調整（地面判定の前に実行）
	bool onSlope = false;
	if (map_) {
		float slopeY;
		if (map_->GetSlopeHeight(transform_.translate, slopeY)) {
			// プレイヤーの足元の高さ
			float playerBottom = transform_.translate.y + mapCollosion_.min.y;
			// プレイヤーの頭頂部の高さ
			float playerTop = transform_.translate.y + mapCollosion_.max.y;
			
			// スロープの高さとの距離を計算
			float distanceToSlope = playerBottom - slopeY;
			
			// スロープの表面付近にいるか、スロープより下にいる場合
			// かつ、プレイヤーの頭頂部がスロープ表面より上にある場合のみ吸着
			if (distanceToSlope <= 0.2f && status_.velocity_Y <= 0.0f && playerTop > slopeY) {
				// スロープに吸着
				transform_.translate.y = slopeY - mapCollosion_.min.y;
				status_.velocity_Y = 0.0f;
				isGrounded_ = true;
				onSlope = true;
			}
		}
	}

	// 地面に着地した場合（下方向の速度がある場合のみ）
	if (isGrounded_ && status_.velocity_Y <= 0.0f) {
		status_.velocity_Y = 0.0f;
		status_.currentJumpCount = 0; // 地面に着いたらジャンプカウントをリセット
		
		// 着地した瞬間の判定（前フレームで空中にいて、今フレームで地面に接触）
		if (!wasGrounded_) {
			// 着地した瞬間の処理
			landingParticle_->Play(transform_.translate, false);
		}
	}

	// Aボタンでジャンプ
	if (MyInput::Trigger(Action::CONFIRM)) {
		if (status_.currentJumpCount < status_.jumpCanCount) {
			status_.velocity_Y = status_.jumpPower;
			status_.currentJumpCount++;
			MyAudio::Play(SE_List::Jump);
		}
	}

	// 重力を適用（スロープ上でない場合、または上昇中の場合）
	if ((!isGrounded_ || status_.velocity_Y > 0.0f) && !onSlope) {
		status_.velocity_Y -= status_.gravity * deltaTime_;
		transform_.translate.y += status_.velocity_Y * deltaTime_;
	}
}

void Player::SlideOnSlope() {
	// しゃがみ中でなければ何もしない
	if (!MyInput::Push(Action::CROUCHING)) {
		return;
	}

	// マップが設定されていなければ何もしない
	if (!map_) {
		return;
	}

	// スロープの傾斜ベクトルを取得
	Vector3 gradient;
	if (!map_->GetSlopeGradient(transform_.translate, gradient)) {
		// スロープ上にいない場合は何もしない
		return;
	}

	// スロープ上にいるか確認（より厳密な判定）
	float slopeY;
	if (!map_->GetSlopeHeight(transform_.translate, slopeY)) {
		return;
	}

	// プレイヤーの足元がスロープ表面付近にあるか確認
	float playerBottom = transform_.translate.y + mapCollosion_.min.y;
	float distanceToSlope = playerBottom - slopeY;

	// スロープ表面から離れすぎている場合は滑らない
	const float maxDistanceToSlide = 0.3f;
	if (distanceToSlope < -0.1f || distanceToSlope > maxDistanceToSlide) {
		return;
	}

	// 傾斜方向に沿って滑る
	// Y成分は無視してXZ平面での移動のみを計算（Y座標はスロープに吸着させる）
	Vector3 slideVelocity = {
		gradient.x * slideSpeed_ * deltaTime_,
		0.0f,  // Y座標はスロープ吸着処理で調整される
		gradient.z * slideSpeed_ * deltaTime_
	};

	// プレイヤーの位置を更新
	transform_.translate.x += slideVelocity.x;
	transform_.translate.z += slideVelocity.z;

	// スロープに吸着させる（Y座標を更新）
	float newSlopeY;
	if (map_->GetSlopeHeight(transform_.translate, newSlopeY)) {
		transform_.translate.y = newSlopeY - mapCollosion_.min.y;
	}

	// AABBの中心も更新
	mapCollosion_.center = transform_.translate;
}

void Player::ResolveMapCollision() {
	if (!map_) return;

	// プレイヤーの現在位置からマップ座標を取得
	uint32_t centerX, centerY, centerZ;
	if (!map_->WorldToMap(transform_.translate, centerX, centerY, centerZ)) {
		// マップ範囲外の場合は何もしない
		return;
	}

	// 最大反復回数（無限ループ防止）
	const int maxIterations = 8;
	const int32_t checkRange = 2;

	// 反復的に衝突を解決
	for (int iteration = 0; iteration < maxIterations; ++iteration) {
		bool hasCollision = false;
		
		// 各軸ごとの最小押し出し量を格納
		struct CollisionInfo {
			float penetration;
			Vector3 pushOut;
			bool isSlopeCollision; // スロープ衝突フラグ
		};

		CollisionInfo bestCollisionX = { (std::numeric_limits<float>::max)(), {0.0f, 0.0f, 0.0f}, false };
		CollisionInfo bestCollisionY = { (std::numeric_limits<float>::max)(), {0.0f, 0.0f, 0.0f}, false };
		CollisionInfo bestCollisionZ = { (std::numeric_limits<float>::max)(), {0.0f, 0.0f, 0.0f}, false };

		// プレイヤーのAABBをワールド座標で取得
		Vector3 playerMin = mapCollosion_.GetMinWorld();
		Vector3 playerMax = mapCollosion_.GetMaxWorld();

		// 現在のマップ座標を再取得
		if (!map_->WorldToMap(transform_.translate, centerX, centerY, centerZ)) {
			break;
		}

		// スロープ上にいるかチェック（各反復で再確認）
		float slopeY;
		bool isOnSlope = map_->GetSlopeHeight(transform_.translate, slopeY);
		float playerBottom = transform_.translate.y + mapCollosion_.min.y;
		float playerTop = transform_.translate.y + mapCollosion_.max.y;
		float distanceToSlope = playerBottom - slopeY;
		bool standingOnSlope = false;
		
		// スロープの上に立っている判定：
		// 1. スロープが存在する
		// 2. プレイヤーの足元がスロープ表面付近にある（下り方向への移動を考慮して許容範囲を広げる）
		// 3. プレイヤーの頭頂部がスロープ表面より上にある（側面衝突を除外）
		if (isOnSlope && distanceToSlope >= -1.0f && distanceToSlope <= 1.0f && playerTop > slopeY) {
			standingOnSlope = true;
		}

		// 周囲のブロックをチェック
		for (int32_t dz = -checkRange; dz <= checkRange; ++dz) {
			for (int32_t dy = -checkRange; dy <= checkRange; ++dy) {
				for (int32_t dx = -checkRange; dx <= checkRange; ++dx) {
					int32_t x = static_cast<int32_t>(centerX) + dx;
					int32_t y = static_cast<int32_t>(centerY) + dy;
					int32_t z = static_cast<int32_t>(centerZ) + dz;

					// 範囲外チェック
					if (x < 0 || y < 0 || z < 0) continue;
					if (!map_->IsInBounds(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z))) continue;

					TileType tileType = map_->GetTile(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
					
					// スロープの側面衝突をチェック（新しい段階的AABB方式）
					// IsSlopeType()を使用してすべてのスロープタイプを判定
					bool isSlopeTile = (tileType == TileType::Slope || 
					                    tileType == TileType::Slope_PlusX || 
					                    tileType == TileType::Slope_MinusX || 
					                    tileType == TileType::Slope_PlusZ || 
					                    tileType == TileType::Slope_MinusZ);
					
					if (isSlopeTile) {
						// ブロックの基本情報を取得
						Vector3 blockWorldPos = map_->MapToWorld(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
						SlopeDirection slopeDir = map_->GetSlopeDirection(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
						
						// デバッグ: スロープブロック検出
						AABB slopeBlockAABB;
						slopeBlockAABB.center = blockWorldPos;
						slopeBlockAABB.min = { -7.5f, -5.0f, -7.5f };
						slopeBlockAABB.max = {  7.5f,  5.0f,  7.5f };
						MyDebugLine::AddShape(slopeBlockAABB, {0.0f, 1.0f, 1.0f, 1.0f}); // シアン色でスロープブロック全体を表示
						
						// スロープを高さ方向に分割して段階的なAABBを作成
						const int slopeSteps = 16; // スロープを10段階に分割
						const float blockHalfHeight = 5.0f; // ブロックの半分の高さ
						const float blockFullHeight = 10.0f; // ブロックの全高さ
						const float blockHalfWidth = 7.5f; // ブロックの半幅
						
						bool hasSlopeSideCollision = false;
						Vector3 slopePushOut = {0.0f, 0.0f, 0.0f};
						float minSlopePenetration = (std::numeric_limits<float>::max)();
						
						// プレイヤーがスロープ表面の上にいるかチェック
						float playerSlopeHeight;
						bool playerIsOnSlopeSurface = false;
						if (map_->GetSlopeHeight(transform_.translate, playerSlopeHeight)) {
							// プレイヤーの足元とスロープ表面の距離
							float distToSurface = playerBottom - playerSlopeHeight;
							// 表面から十分に上にいる場合（スロープの上に立っている）
							if (distToSurface >= -0.15f && distToSurface <= 0.15f) {
								playerIsOnSlopeSurface = true;
							}
						}
						
						// 各段階でAABBを生成してチェック
						for (int step = 0; step < slopeSteps; ++step) {
							// この段階の高さ割合（0.0 = 底, 1.0 = 頂上）
							float stepHeightStart = static_cast<float>(step) / static_cast<float>(slopeSteps);
							float stepHeightEnd = static_cast<float>(step + 1) / static_cast<float>(slopeSteps);
							
							// この段階のAABBを構築
							AABB stepAABB;
							stepAABB.center = {0.0f, 0.0f, 0.0f}; // 後で設定
							stepAABB.min = {0.0f, 0.0f, 0.0f};
							stepAABB.max = {0.0f, 0.0f, 0.0f};
							
							// Y座標の範囲（スロープブロックの底面を基準）
							float yMin = blockWorldPos.y - blockHalfHeight + stepHeightStart * blockFullHeight;
							float yMax = blockWorldPos.y - blockHalfHeight + stepHeightEnd * blockFullHeight;
							
							// XZ座標の範囲（スロープの向きに応じて変化）
							float xMin, xMax, zMin, zMax;
							
							switch (slopeDir) {
								case SlopeDirection::PlusX: // X+方向に登る
									// 低い方（X-側）から高い方（X+側）へ
									xMin = blockWorldPos.x - blockHalfWidth + stepHeightStart * (blockHalfWidth * 2.0f);
									xMax = blockWorldPos.x - blockHalfWidth + stepHeightEnd * (blockHalfWidth * 2.0f);
									zMin = blockWorldPos.z - blockHalfWidth;
									zMax = blockWorldPos.z + blockHalfWidth;
									break;
									
								case SlopeDirection::MinusX: // X-方向に登る
									// 低い方（X+側）から高い方（X-側）へ
									xMin = blockWorldPos.x + blockHalfWidth - stepHeightEnd * (blockHalfWidth * 2.0f);
									xMax = blockWorldPos.x + blockHalfWidth - stepHeightStart * (blockHalfWidth * 2.0f);
									zMin = blockWorldPos.z - blockHalfWidth;
									zMax = blockWorldPos.z + blockHalfWidth;
									break;
									
								case SlopeDirection::PlusZ: // Z+方向に登る
									// 低い方（Z-側）から高い方（Z+側）へ
									xMin = blockWorldPos.x - blockHalfWidth;
									xMax = blockWorldPos.x + blockHalfWidth;
									zMin = blockWorldPos.z - blockHalfWidth + stepHeightStart * (blockHalfWidth * 2.0f);
									zMax = blockWorldPos.z - blockHalfWidth + stepHeightEnd * (blockHalfWidth * 2.0f);
									break;
									
								case SlopeDirection::MinusZ: // Z-方向に登る
									// 低い方（Z+側）から高い方（Z-側）へ
									xMin = blockWorldPos.x - blockHalfWidth;
									xMax = blockWorldPos.x + blockHalfWidth;
									zMin = blockWorldPos.z + blockHalfWidth - stepHeightEnd * (blockHalfWidth * 2.0f);
									zMax = blockWorldPos.z + blockHalfWidth - stepHeightStart * (blockHalfWidth * 2.0f);
									break;
							}
							
							// AABBを設定
							stepAABB.center = {
								(xMin + xMax) / 2.0f,
								(yMin + yMax) / 2.0f,
								(zMin + zMax) / 2.0f
							};
							stepAABB.min = {
								xMin - stepAABB.center.x,
								-10.0f,
								zMin - stepAABB.center.z
							};
							stepAABB.max = {
								xMax - stepAABB.center.x,
								-0.5f,
								zMax - stepAABB.center.z
							};
							
							// デバッグ可視化（段階ごとに異なる色）- 常に表示
							float colorIntensity = stepHeightStart;
							MyDebugLine::AddShape(stepAABB, {1.0f, colorIntensity, 0.0f, 0.3f});
							
							// プレイヤーとの衝突判定
							bool isHit = Collision::IsHit(mapCollosion_, stepAABB);
							
							// デバッグ: 衝突した段階を強調表示
							if (isHit) {
								MyDebugLine::AddShape(stepAABB, {1.0f, 0.0f, 1.0f, 0.8f}); // マゼンタで衝突を表示
							}
							
							if (!isHit) {
								continue;
							}
							
							// スロープ表面の上にいる場合はスキップ
							if (playerIsOnSlopeSurface) {
								continue;
							}
							
							// XZ方向のみの押し出し量を計算
							Vector3 stepMin = stepAABB.GetMinWorld();
							Vector3 stepMax = stepAABB.GetMaxWorld();

							float penetrationX = (std::min)(playerMax.x - stepMin.x, stepMax.x - playerMin.x);
							float penetrationZ = (std::min)(playerMax.z - stepMin.z, stepMax.z - playerMin.z);
							
							// スロープの向きに基づいて優先軸を決定
							bool shouldPushX = false;
							bool shouldPushZ = false;
							
							switch (slopeDir) {
								case SlopeDirection::PlusX:
								case SlopeDirection::MinusX:
									// X軸方向のスロープなので、Z軸で押し出す
									shouldPushZ = true;
									break;
								case SlopeDirection::PlusZ:
								case SlopeDirection::MinusZ:
									// Z軸方向のスロープなので、X軸で押し出す
									shouldPushX = true;
									break;
							}
							
							// 適切な軸で押し出し
							if (shouldPushX && penetrationX < minSlopePenetration) {
								minSlopePenetration = penetrationX;
								if (transform_.translate.x < stepAABB.center.x) {
									slopePushOut = {-penetrationX, 0.0f, 0.0f};
								} else {
									slopePushOut = {penetrationX, 0.0f, 0.0f};
								}
								hasSlopeSideCollision = true;
							} else if (shouldPushZ && penetrationZ < minSlopePenetration) {
								minSlopePenetration = penetrationZ;
								if (transform_.translate.z < stepAABB.center.z) {
									slopePushOut = {0.0f, 0.0f, -penetrationZ};
								} else {
									slopePushOut = {0.0f, 0.0f, penetrationZ};
								}
								hasSlopeSideCollision = true;
							}
						}
						
						// スロープ側面衝突が検出された場合
						if (hasSlopeSideCollision) {
							hasCollision = true;
							
							// 適切な軸に押し出しを適用
							if (slopePushOut.x != 0.0f) {
								if (minSlopePenetration < bestCollisionX.penetration) {
									bestCollisionX.penetration = minSlopePenetration;
									bestCollisionX.pushOut = slopePushOut;
									bestCollisionX.isSlopeCollision = true;
								}
							} else if (slopePushOut.z != 0.0f) {
								if (minSlopePenetration < bestCollisionZ.penetration) {
									bestCollisionZ.penetration = minSlopePenetration;
									bestCollisionZ.pushOut = slopePushOut;
									bestCollisionZ.isSlopeCollision = true;
								}
							}
						}
						
						// スロープの上に立っている場合はスキップ（従来の処理）
						continue;
					}

					// Normalブロックのみをチェック（スロープは上で処理済み）
					if (tileType != TileType::Normal) continue;

					// ブロックのAABBを取得
					Vector3 blockWorldPos = map_->MapToWorld(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
					AABB blockAABB;
					blockAABB.center = blockWorldPos;
					blockAABB.min = { -7.5f, -5.0f, -7.5f };
					blockAABB.max = {  7.5f,  5.0f,  7.5f };

					// 衝突判定
					if (!Collision::IsHit(mapCollosion_, blockAABB)) continue;

					// スロープ上にいる場合は、スロープの下にあるブロックを無視
					if (standingOnSlope) {
						// ブロックの上面とプレイヤーの足元の高さを比較
						float blockTop = blockAABB.GetMaxWorld().y;
						
						// スロープの高さより下のブロックは全て無視
						// slopeYはスロープ表面の高さなので、それより下のブロックは無視
						if (blockTop <= slopeY + 1.0f) {
							continue;
						}
						
						// プレイヤーの足元より下にあるブロックも無視（横方向の衝突）
						if (blockTop <= playerBottom + 1.0f) {
							continue;
						}
						
						// 追加チェック: ブロックがスロープの真下にある場合は無視
						// スロープの高さとブロックの上面を比較
						// ブロックの上面がスロープ表面-許容値より下にある場合は無視
						float slopeHeightAtBlockPos;
						if (map_->GetSlopeHeight(blockWorldPos, slopeHeightAtBlockPos)) {
							// このブロック位置でのスロープ高さより下にあるブロックは無視
							if (blockTop <= slopeHeightAtBlockPos) {
								continue;
							}
						}
					}

					// 衝突を検出
					hasCollision = true;

					// 衝突している場合、押し出し量を計算
					Vector3 blockMin = blockAABB.GetMinWorld();
					Vector3 blockMax = blockAABB.GetMaxWorld();

					// 各軸での重なり量を計算
					float overlapX = (std::min)(playerMax.x - blockMin.x, blockMax.x - playerMin.x);
					float overlapY = (std::min)(playerMax.y - blockMin.y, blockMax.y - playerMin.y);
					float overlapZ = (std::min)(playerMax.z - blockMin.z, blockMax.z - playerMin.z);

					// X軸の押し出しを評価
					if (overlapX < bestCollisionX.penetration) {
						bestCollisionX.penetration = overlapX;
						bestCollisionX.isSlopeCollision = false;
						if (transform_.translate.x < blockWorldPos.x) {
							bestCollisionX.pushOut = { -overlapX, 0.0f, 0.0f };
						} else {
							bestCollisionX.pushOut = { overlapX, 0.0f, 0.0f };
						}
					}

					// Y軸の押し出しを評価
					if (overlapY < bestCollisionY.penetration) {
						bestCollisionY.penetration = overlapY;
						bestCollisionY.isSlopeCollision = false;
						if (transform_.translate.y < blockWorldPos.y) {
							bestCollisionY.pushOut = { 0.0f, -overlapY, 0.0f };
						} else {
							bestCollisionY.pushOut = { 0.0f, overlapY, 0.0f };
						}
					}

					// Z軸の押し出しを評価
					if (overlapZ < bestCollisionZ.penetration) {
						bestCollisionZ.penetration = overlapZ;
						bestCollisionZ.isSlopeCollision = false;
						if (transform_.translate.z < blockWorldPos.z) {
							bestCollisionZ.pushOut = { 0.0f, 0.0f, -overlapZ };
						} else {
							bestCollisionZ.pushOut = { 0.0f, 0.0f, overlapZ };
						}
					}
				}
			}
		}

		// 衝突がない場合は終了
		if (!hasCollision) {
			break;
		}

		// 各軸で最も浅い押し出しを適用
		// スロープ側面衝突がある場合は、それを最優先
		Vector3 finalPushOut = { 0.0f, 0.0f, 0.0f };
		bool pushedOut = false;

		// スロープ側面衝突を最優先で処理
		if (bestCollisionX.isSlopeCollision || bestCollisionZ.isSlopeCollision) {
			// スロープ側面衝突がある場合は、横方向（XまたはZ）のみを処理
			if (bestCollisionX.isSlopeCollision && bestCollisionZ.isSlopeCollision) {
				// 両方ともスロープ衝突の場合、より小さい押し出しを選択
				if (bestCollisionX.penetration <= bestCollisionZ.penetration) {
					finalPushOut = bestCollisionX.pushOut;
				} else {
					finalPushOut = bestCollisionZ.pushOut;
				}
				pushedOut = true;
			} else if (bestCollisionX.isSlopeCollision) {
				finalPushOut = bestCollisionX.pushOut;
				pushedOut = true;
			} else if (bestCollisionZ.isSlopeCollision) {
				finalPushOut = bestCollisionZ.pushOut;
				pushedOut = true;
			}
		}

		// スロープ衝突がない場合、通常の優先順位で処理
		if (!pushedOut) {
			// Y軸の押し出しを最優先
			if (bestCollisionY.penetration < (std::numeric_limits<float>::max)()) {
				float threshold = 0.5f; // Y軸優先のしきい値
				if (bestCollisionY.penetration <= bestCollisionX.penetration * threshold &&
				    bestCollisionY.penetration <= bestCollisionZ.penetration * threshold) {
					finalPushOut = bestCollisionY.pushOut;
					pushedOut = true;
					
					// Y軸方向の押し出しの場合、速度と地面状態を処理
					if (finalPushOut.y < 0.0f) {
						status_.velocity_Y = 0.0f;
					} else if (finalPushOut.y > 0.0f && status_.velocity_Y > 0.0f) {
						status_.velocity_Y = 0.0f;
					}
				}
			}

			// Y軸が適用されなかった場合、XとZの最小値を使用
			if (!pushedOut) {
				if (bestCollisionX.penetration <= bestCollisionZ.penetration &&
				    bestCollisionX.penetration < (std::numeric_limits<float>::max)()) {
					finalPushOut = bestCollisionX.pushOut;
					pushedOut = true;
				} else if (bestCollisionZ.penetration < (std::numeric_limits<float>::max)()) {
					finalPushOut = bestCollisionZ.pushOut;
					pushedOut = true;
				}
			}
		}

		// 押し出しを適用
		if (pushedOut) {
			transform_.translate.x += finalPushOut.x;
			transform_.translate.y += finalPushOut.y;
			transform_.translate.z += finalPushOut.z;

			// AABBの中心も更新
			mapCollosion_.center = transform_.translate;

			// プレイヤーのAABBを更新（次の反復のため）
			playerMin = mapCollosion_.GetMinWorld();
			playerMax = mapCollosion_.GetMaxWorld();
			
			// スロープ側面衝突で押し出された場合、次の反復をスキップして即座に終了
			// （ガタガタを防ぐため）
			if (bestCollisionX.isSlopeCollision || bestCollisionZ.isSlopeCollision) {
				break;
			}
		} else {
			break;
		}
	}
}

bool Player::IsGroundedOnMap() {
	if (!map_) return false;

	// プレイヤーの足元の少し下の位置をチェック
	const float groundCheckDistance = 0.15f;
	
	// プレイヤーの足元のAABBを作成（少し下に拡張）
	AABB groundCheckAABB;
	groundCheckAABB.center = transform_.translate;
	groundCheckAABB.min = mapCollosion_.min;
	groundCheckAABB.max = mapCollosion_.max;
	// 足元を少し下に拡張
	groundCheckAABB.min.y -= groundCheckDistance;

	// プレイヤーの現在位置からマップ座標を取得
	uint32_t centerX, centerY, centerZ;
	if (!map_->WorldToMap(transform_.translate, centerX, centerY, centerZ)) {
		return false;
	}

	// 周囲のブロックをチェック（±2ブロック範囲、主に下方向）
	const int32_t checkRange = 2;
	
	for (int32_t dz = -checkRange; dz <= checkRange; ++dz) {
		for (int32_t dy = -checkRange; dy <= 0; ++dy) { // 下方向のみチェック
			for (int32_t dx = -checkRange; dx <= checkRange; ++dx) {
				int32_t x = static_cast<int32_t>(centerX) + dx;
				int32_t y = static_cast<int32_t>(centerY) + dy;
				int32_t z = static_cast<int32_t>(centerZ) + dz;

				// 範囲外チェック
				if (x < 0 || y < 0 || z < 0) continue;
				if (!map_->IsInBounds(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z))) continue;

				TileType tileType = map_->GetTile(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
				
				// Normalブロックのチェック
				if (tileType == TileType::Normal) {
					// ブロックのAABBを取得
					Vector3 blockWorldPos = map_->MapToWorld(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
					AABB blockAABB;
					blockAABB.center = blockWorldPos;
					blockAABB.min = { -7.5f, -5.0f, -7.5f };
					blockAABB.max = {  7.5f,  5.0f,  7.5f };

					// 足元チェック用AABBとの衝突判定
					if (Collision::IsHit(groundCheckAABB, blockAABB)) {
						// ブロックが足元にある場合、そのブロックの上面がプレイヤーの足元より少し下にあるかチェック
						float blockTop = blockAABB.GetMaxWorld().y;
						float playerBottom = mapCollosion_.GetMinWorld().y;
						
						// ブロックの上面がプレイヤーの足元付近にある場合のみ地面と判定
						// より厳密な判定：ブロックの上面がプレイヤーの足元のすぐ下にある場合のみ
						float distance = playerBottom - blockTop;
						if (distance >= -0.01f && distance <= groundCheckDistance) {
							return true;
						}
					}
				}
				// スロープのチェック
				else if (tileType == TileType::Slope) {
					// スロープの場合は、スロープ上のY座標を計算して判定
					float slopeY;
					if (map_->GetSlopeHeight(transform_.translate, slopeY)) {
						float playerBottom = transform_.translate.y + mapCollosion_.min.y;
						float distance = playerBottom - slopeY;
						// スロープの表面付近にいる場合は地面と判定
						if (distance >= -0.01f && distance <= groundCheckDistance) {
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

Vector3 Player::GetDirectionToEnemy() const {
	// EnemyManagerが設定されていない場合はゼロベクトルを返す
	if (!enemyManager_) {
		return Vector3{ 0.0f, 0.0f, 0.0f };
	}

	const auto& enemies = enemyManager_->GetEnemies();
	
	// 敵がいない場合はゼロベクトルを返す
	if (enemies.empty()) {
		return Vector3{ 0.0f, 0.0f, 0.0f };
	}

	const Vector3& playerPos = transform_.translate;

	// 距離と敵のペアを格納するベクトル
	std::vector<std::pair<float, const std::unique_ptr<Enemy>*>> enemyDistances;
	enemyDistances.reserve(enemies.size());

	// 全ての敵をチェックして距離を計算
	for (const auto& enemy : enemies) {
		// 死んでいる敵はスキップ
		if (!enemy->IsAlive()) {
			continue;
		}

		const Vector3& enemyPos = enemy->GetPosition();
		
		// 敵へのベクトルを計算
		Vector3 vectorToEnemy = {
			enemyPos.x - playerPos.x,
			enemyPos.y - playerPos.y,
			enemyPos.z - playerPos.z
		};

		// 距離の二乗を計算（sqrtを避けるため）
		float distanceSquared = 
			vectorToEnemy.x * vectorToEnemy.x +
			vectorToEnemy.y * vectorToEnemy.y +
			vectorToEnemy.z * vectorToEnemy.z;

		enemyDistances.push_back({distanceSquared, &enemy});
	}

	// 敵がいない場合（全て死んでいる場合）
	if (enemyDistances.empty()) {
		return Vector3{ 0.0f, 0.0f, 0.0f };
	}

	// 距離でソート（近い順）
	std::sort(enemyDistances.begin(), enemyDistances.end(),
		[](const auto& a, const auto& b) {
			return a.first < b.first;
		});

	// 近い敵10体（またはそれ以下）から選択
	const size_t candidateCount = std::min(size_t(10), enemyDistances.size());
	
	// ランダムに1体選択
	int randomIndex = MyRand::Int(0, static_cast<int>(candidateCount) - 1);
	const std::unique_ptr<Enemy>* selectedEnemy = enemyDistances[randomIndex].second;

	// 選択された敵へのベクトルを計算
	const Vector3& enemyPos = (*selectedEnemy)->GetPosition();
	Vector3 vectorToEnemy = {
		enemyPos.x - playerPos.x,
		enemyPos.y - playerPos.y,
		enemyPos.z - playerPos.z
	};

	return vectorToEnemy;
}

Vector3 Player::GetDirectionToClosestEnemy() const {
	// EnemyManagerが設定されていない場合はゼロベクトルを返す
	if (!enemyManager_) {
		return Vector3{ 0.0f, 0.0f, 0.0f };
	}

	const auto& enemies = enemyManager_->GetEnemies();
	
	// 敵がいない場合はゼロベクトルを返す
	if (enemies.empty()) {
		return Vector3{ 0.0f, 0.0f, 0.0f };
	}

	float minDistanceSquared = (std::numeric_limits<float>::max)();
	Vector3 vectorToNearest = { 0.0f, 0.0f, 0.0f };
	const Vector3& playerPos = transform_.translate;

	// 全ての敵をチェックして最も近い敵を見つける
	for (const auto& enemy : enemies) {
		// 死んでいる敵はスキップ
		if (!enemy->IsAlive()) {
			continue;
		}

		const Vector3& enemyPos = enemy->GetPosition();
		
		// 敵へのベクトルを計算
		Vector3 vectorToEnemy = {
			enemyPos.x - playerPos.x,
			enemyPos.y - playerPos.y,
			enemyPos.z - playerPos.z
		};

		// 距離の二乗を計算（sqrtを避けるため）
		float distanceSquared = 
			vectorToEnemy.x * vectorToEnemy.x +
			vectorToEnemy.y * vectorToEnemy.y +
			vectorToEnemy.z * vectorToEnemy.z;

		// より近い敵が見つかった場合
		if (distanceSquared < minDistanceSquared) {
			minDistanceSquared = distanceSquared;
			vectorToNearest = vectorToEnemy;
		}
	}

	return vectorToNearest;
}

float Player::GetDistanceToNearestEnemy() const {
	Vector3 vectorToNearest = GetDirectionToEnemy();
	
	// ゼロベクトルの場合は無限大を返す
	if (vectorToNearest.x == 0.0f && vectorToNearest.y == 0.0f && vectorToNearest.z == 0.0f) {
		return (std::numeric_limits<float>::max)();
	}

	return vectorToNearest.Length();
}

void Player::SetCurrentHP(int hp) {
	status_.currentHP = std::clamp(hp, 0, status_.maxHP);
}

void Player::TakeDamage(int damage) {
	// 無敵時間中はダメージを受けない
	if (!invincibilityTimer_.IsActive()) {
		invincibilityTimer_.Start(2.0f, false); // 0.5秒の無敵時間
		status_.currentHP -= damage;
		
		MyAudio::Play(SE_List::PlayerDamage);

		// HPが0以下になったら0にクランプ
		if (status_.currentHP < 0) {
			status_.currentHP = 0;
		}
		
		// TODO: ここで死亡処理やダメージエフェクトを追加可能
	}
}

void Player::AddExp(int exp) {
	status_.currentExp += int(float(exp) * status_.expMultiply);
	
	// レベルアップチェック
	while (status_.currentExp >= status_.expToNextLevel) {
		status_.currentExp -= status_.expToNextLevel;
		status_.level++;
		// 次のレベルに必要な経験値を増やす（例：1.5倍）
		status_.expToNextLevel = static_cast<int>(status_.expToNextLevel * 1.1f);
		
		// レベルアップしたらアップグレード選択画面を表示
		if (upgradeManager_) {
			upgradeManager_->Upgrade();
		}
	}
}

void Player::AddMoney(int money) {
	status_.nowMoney += int(float(money) * status_.moneyMultiply);
	if (status_.nowMoney < 0) {
		status_.nowMoney = 0;
	}
}

bool Player::SubtractMoney(int money) {
	if (status_.nowMoney >= money) {
		status_.nowMoney -= money;
		return true;
	}
	return false;
}

float Player::GetGroundHeight() const {
	if (!map_) {
		return groundLevel_; // フォールバック値を返す
	}

	// スロープ上にいる場合はスロープの高さを返す
	float slopeY;
	if (map_->GetSlopeHeight(transform_.translate, slopeY)) {
		return slopeY;
	}

	// プレイヤーの現在位置からマップ座標を取得
	uint32_t centerX, centerY, centerZ;
	if (!map_->WorldToMap(transform_.translate, centerX, centerY, centerZ)) {
		return groundLevel_; // マップ範囲外の場合はフォールバッグ値
	}

	// 周囲と下方向のブロックをチェック
	const int32_t checkRange = 2;
	float highestGroundY = -(std::numeric_limits<float>::max)(); // 最も高い地面を見つける
	bool foundGround = false;

	for (int32_t dz = -checkRange; dz <= checkRange; ++dz) {
		for (int32_t dy = -checkRange; dy <= checkRange; ++dy) {
			for (int32_t dx = -checkRange; dx <= checkRange; ++dx) {
				int32_t x = static_cast<int32_t>(centerX) + dx;
				int32_t y = static_cast<int32_t>(centerY) + dy;
				int32_t z = static_cast<int32_t>(centerZ) + dz;

				// 範囲外チェック
				if (x < 0 || y < 0 || z < 0) continue;
				if (!map_->IsInBounds(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z))) continue;

				TileType tileType = map_->GetTile(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));

				// Normalブロックのチェック
				if (tileType == TileType::Normal) {
					Vector3 blockWorldPos = map_->MapToWorld(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
					
					// ブロックの上面の高さを計算
					// blockWorldPosはブロックの中心なので、+5.0fで上面の高さになる
					float blockTopY = blockWorldPos.y + 5.0f;
					
					// プレイヤーの真下または真下付近にあるブロックのみを考慮
					float xzDistance = std::sqrt(
						(transform_.translate.x - blockWorldPos.x) * (transform_.translate.x - blockWorldPos.x) +
						(transform_.translate.z - blockWorldPos.z) * (transform_.translate.z - blockWorldPos.z)
					);
					
					// プレイヤーの足元より下にあり、XZ平面で近い位置にあるブロックのみを考慮
					if (blockTopY <= transform_.translate.y && xzDistance <= 10.0f) {
						if (blockTopY > highestGroundY) {
							highestGroundY = blockTopY;
							foundGround = true;
						}
					}
				}
			}
		}
	}

	// 地面が見つかった場合はその高さを返す
	if (foundGround) {
		return highestGroundY;
	}

	// 地面が見つからない場合はフォールバック値
	return groundLevel_;
}