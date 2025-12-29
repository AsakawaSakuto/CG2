#include "player.h"
#include "GameObject/EnemyManager/EnemyManager.h"
#include "Map/Map3D.h"
#include "Map/TreeManager/TreeManager.h"
#include <cmath>
#include <algorithm>
#include <limits>

void Player::PostFrameCleanup() {
	weaponManager_->PostFrameCleanup();
}

void Player::Initialize() {

	transform_.SetAllScale(1.5);
	transform_.translate = { 10.0f,100.0f,10.0f };

	model_->Initialize("animation/human/walk.gltf");
	model_->UseLight(false);

	moveParticle_->Initialize();
	moveParticle_->LoadJson("playerMove");
	landingParticle_->Initialize();
	landingParticle_->LoadJson("playerLanding");

	weaponManager_->Initialize();

	// ステータスの初期化
	status_.currentHP = status_.maxHP;
	status_.currentExp = 0;
	status_.level = 1;

	// AABBの初期化（新仕様: center + min/maxのローカルオフセット）
	// center は transform_.translate で毎フレーム更新されるため、ここでは初期化不要
	// min/max はローカル空間でのオフセットとして設定
	mapCollosion_.center = { 0.0f, 0.0f, 0.0f }; // 初期値（Update()で更新される）
	mapCollosion_.min = { -0.5f, 0.0f, -0.5f };  // centerからのローカルオフセット
	mapCollosion_.max = {  0.5f, 0.25f,  0.5f };  // centerからのローカルオフセット
}

void Player::Update() {

	Move();
	Jump();

	// OBBの中心をプレイヤーの位置に設定
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

	MyDebugLine::AddShape(mapCollosion_, {1.0f,0.0f,0.0f,1.0f});

	directionToEnemy_ = GetDirectionToEnemy();

	expGetRangeTransform_.translate = transform_.translate;
	expGetRangeTransform_.scale = { 7.0f, 1.0f, 7.0f };

	model_->Update();

	moveParticle_->SetOffSet({ 0.0f, -0.2f, 0.0f });
	moveParticle_->Update();
	landingParticle_->Update();

	weaponManager_->SetDirectionToEnemy(directionToEnemy_);
	weaponManager_->SetPlayerPosition(transform_.translate);
	//weaponManager_->Update();

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
	ImGui::DragFloat("Move Speed", &status_.moveSpeed, 0.1f, 0.1f, 20.0f);
	ImGui::DragFloat3("Position", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.01f);
	
	// ジャンプ設定
	ImGui::Separator();
	ImGui::Text("Jump Settings");
	ImGui::DragInt("Jump Can Count", &status_.jumpCanCount, 1, 1, 5);
	ImGui::DragFloat("Jump Power", &status_.jumpPower, 0.1f, 1.0f, 20.0f);
	ImGui::DragFloat("Gravity", &status_.gravity, 0.1f, 1.0f, 50.0f);
	ImGui::DragFloat("Ground Level", &groundLevel_, 0.1f, -10.0f, 10.0f);
	ImGui::Text("Current Jump Count: %d / %d", status_.currentJumpCount, status_.jumpCanCount);
	ImGui::Text("Is Grounded: %s", isGrounded_ ? "Yes" : "No");
	ImGui::Text("Velocity Y: %.2f", status_.velocity_Y);
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
		
		if (!moveParticle_->IsPlaying()) {
			moveParticle_->Play(transform_.translate, false);
		}
	} else {
		moveParticle_->Stop();
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
		}
	}

	// 重力を適用（スロープ上でない場合、または上昇中の場合）
	if ((!isGrounded_ || status_.velocity_Y > 0.0f) && !onSlope) {
		status_.velocity_Y -= status_.gravity * deltaTime_;
		transform_.translate.y += status_.velocity_Y * deltaTime_;
	}
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
		// 2. プレイヤーの足元がスロープ表面付近にある
		// 3. プレイヤーの頭頂部がスロープ表面より上にある（側面衝突を除外）
		if (isOnSlope && distanceToSlope >= -0.1f && distanceToSlope <= 0.1f && playerTop > slopeY) {
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

					// スロープ上にいる場合は、横方向と下方向の衝突を無視
					if (standingOnSlope) {
						// ブロックの上面とプレイヤーの足元の高さを比較
						float blockTop = blockAABB.GetMaxWorld().y;
						
						// スロープの高さより下のブロックは全て無視
						if (blockTop <= slopeY + 1.0f) {
							continue;
						}
						
						// プレイヤーの足元より下にあるブロックも無視（横方向の衝突）
						if (blockTop <= playerBottom) {
							continue;
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

void Player::AddExp(int exp) {
	status_.currentExp += int(float(exp) * status_.expMultiply);
	
	// レベルアップチェック
	while (status_.currentExp >= status_.expToNextLevel) {
		status_.currentExp -= status_.expToNextLevel;
		status_.level++;
		// 次のレベルに必要な経験値を増やす（例：1.5倍）
		status_.expToNextLevel = static_cast<int>(status_.expToNextLevel * 1.1f);
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