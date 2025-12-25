#include "player.h"
#include "GameObject/EnemyManager/EnemyManager.h"
#include <cmath>
#include <algorithm>
#include <limits>

void Player::PostFrameCleanup() {
	weaponManager_->PostFrameCleanup();
}

void Player::Initialize() {

	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.translate = { 10.0f,10.0f,10.0f };

	model_->Initialize("animation/human/walk.gltf");
	model_->UseLight(false);

	moveParticle_->Initialize();
	moveParticle_->LoadJson("playerMove");
	landingParticle_->Initialize();
	landingParticle_->LoadJson("playerLanding");

	weaponManager_->Initialize();

	// ステータスの初期化
	status_.currentHP_ = status_.maxHP_;
	status_.currentExp_ = 0;
	status_.level_ = 1;

	// AABBの初期化（ローカルオフセットを設定）
	// 中心から見て、下方向に0.0f、上方向に2.0f伸びる
	mapCollosion_.min = { -0.5f, 0.0f, -0.5f }; 
	mapCollosion_.max = { 0.5f, 2.0f, 0.5f };
}

void Player::Update() {

	Move();
	Jump();

	// AABBの中心をプレイヤーの位置に設定（超シンプル！）
	mapCollosion_.center = transform_.translate;
	MyDebugLine::AddShape(mapCollosion_, {1.0f,0.0f,0.0f,1.0f});

	directionToEnemy_ = GetDirectionToEnemy();

	//transform_.translate.x = std::clamp(transform_.translate.x, -49.5f, 49.5f);
	//transform_.translate.z = std::clamp(transform_.translate.z, -49.5f, 49.5f);

	expGetRangeTransform_.translate = transform_.translate;
	expGetRangeTransform_.scale = { 7.0f, 1.0f, 7.0f };

	model_->Update();

	moveParticle_->SetOffSet({ 0.0f, -0.2f, 0.0f });
	moveParticle_->Update();
	landingParticle_->Update();

	weaponManager_->SetDirectionToEnemy(directionToEnemy_);
	weaponManager_->SetPlayerPosition(transform_.translate);
	weaponManager_->Update();

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = collisionRadius_;

	expItemStateChangeCollision_.center = transform_.translate;
	expItemStateChangeCollision_.radius = expItemStateChangeRadius_;

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

	model_->Draw(camera, transform_);

	//expItemGetRange_->Draw(camera, expGetRangeTransform_);

	weaponManager_->Draw(camera);

	moveParticle_->Draw(camera);
	landingParticle_->Draw(camera);
}

void Player::DrawImGui() {
#ifdef USE_IMGUI
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
#endif
	//landingParticle_->DrawImGui("move Particle");
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
			moveParticle_->Play(transform_.translate, false);
		}
	} else {
		moveParticle_->Stop();
	}
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
	// stickY > 0: 上に倒す → カメラが向いている方向（前方向）に移動
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

	// 地面にいるかのチェック
	if (transform_.translate.y <= groundLevel_) {
		transform_.translate.y = groundLevel_;
		status_.velocity_Y_ = 0.0f;
		isGrounded_ = true;
		status_.currentJumpCount_ = 0; // 地面に着いたらジャンプカウントをリセット
		
		// 着地した瞬間の判定（前フレームで空中にいて、今フレームで地面に接触）
		if (!wasGrounded_) {
			// 着地した瞬間の処理
			landingParticle_->Play(transform_.translate, false);
		}
	} else {
		isGrounded_ = false;
	}

	// Aボタンでジャンプ
	if (MyInput::Trigger(Action::CONFIRM)) {
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
	status_.currentHP_ = std::clamp(hp, 0, status_.maxHP_);
}

void Player::AddExp(int exp) {
	status_.currentExp_ += exp;
	
	// レベルアップチェック
	while (status_.currentExp_ >= status_.expToNextLevel_) {
		status_.currentExp_ -= status_.expToNextLevel_;
		status_.level_++;
		// 次のレベルに必要な経験値を増やす（例：1.5倍）
		status_.expToNextLevel_ = static_cast<int>(status_.expToNextLevel_ * 1.1f);
	}
}