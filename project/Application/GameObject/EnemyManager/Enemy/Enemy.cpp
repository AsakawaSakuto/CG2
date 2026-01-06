#include "Enemy.h"
#include "Map/Map3D.h"
#include "Utility/Collision/Collision.h"
#include <limits>

void Enemy::Initialize() {
	
	transform_.scale = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,0.0f,0.0f };

	//model_->Initialize("Animation/human/lowWalk.gltf");
	model_->Initialize("enemy/enemy.obj");
	// フラスタムカリングを有効化（画面外の敵の更新・描画をスキップ）
	//model_->SetUpdateFrustumCulling(true);
	model_->SetDrawFrustumCulling(true);

	moveSpeed_ = 2.0f;
	collicionRadius_ = 0.5f;

	scaleTimer_.Start(0.5f, false);
	
	// AABBの初期化（プレイヤーと同様の構造）
	mapCollosion_.center = { 0.0f, 0.0f, 0.0f };
	mapCollosion_.min = { -0.5f, 0.0f, -0.5f };
	mapCollosion_.max = { 0.5f, 0.25f, 0.5f };
	
	// 物理パラメータの初期化
	velocity_Y = 0.0f;
	gravity_ = 20.0f;
	isGrounded_ = false;
	wallClimbSpeed_ = 25.0f; // 上昇速度を速くする
}

void Enemy::Update() {
	
	if (status_.hp <= 0) {
		Dead(); 
		return;
	}

	Move();
	
	// AABBの中心をエネミーの位置に設定
	mapCollosion_.center = transform_.translate;
	
	// マップとの衝突解決を実行
	if (map_) {
		ResolveMapCollision();
	}

	transform_.scale = MyEasing::Lerp(Vector3({0.0f,0.0f,0.0f}), Vector3({ 1.0f,1.0f,1.0f }), 
		scaleTimer_.GetProgress(), EaseType::Linear);

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = collicionRadius_;

	scaleTimer_.Update();

	if (invicibilityTimer_.IsActive() && invicibilityTimer_.GetProgress() < 0.9f) {
		model_->SetColor3({ 0.1f,0.1f,0.1f });
	} else {
		model_->SetColor3({ 1.0f,1.0f,1.0f });
	}

	invicibilityTimer_.Update();
	if (invicibilityTimer_.IsFinished()) {
		invicibilityTimer_.Reset();
	}

#ifdef USE_IMGUI
	// デバッグ描画はF1キーで切り替え（パフォーマンス向上のため）
	if (MyInput::PushKey(DIK_F1)) {
		MyDebugLine::AddShape(sphereCollision_);
		MyDebugLine::AddShape(mapCollosion_, {0.0f, 1.0f, 0.0f, 1.0f});
	}
#endif
}

void Enemy::Draw(Camera camera) {
	if (scaleTimer_.GetProgress() >= 0.1f) {
		model_->Draw(camera, transform_);
	}
}

void Enemy::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Enemy Debug");
	
	// 基本情報
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", 
		transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("Velocity Y: %.2f", velocity_Y);
	ImGui::Text("IsGrounded: %s", isGrounded_ ? "Yes" : "No");
	
	// スロープ情報
	float slopeY = 0.0f;
	bool onSlope = false;
	if (map_) {
		onSlope = map_->GetSlopeHeight(transform_.translate, slopeY);
		if (onSlope) {
			float playerBottom = transform_.translate.y + mapCollosion_.min.y;
			float distanceToSlope = playerBottom - slopeY;
			
			ImGui::Separator();
			ImGui::Text("ON SLOPE");
			ImGui::Text("Slope Y: %.2f", slopeY);
			ImGui::Text("Player Bottom: %.2f", playerBottom);
			ImGui::Text("Distance to Slope: %.2f", distanceToSlope);
		}
	}
	
	ImGui::End();
#endif
}

void Enemy::Move() {
	// マップとの地面判定を使用
	if (map_) {
		isGrounded_ = IsGroundedOnMap();
	}
	
	// スロープ上にいる場合はY座標を調整（プレイヤーと同じ処理）
	bool onSlope = false;
	if (map_) {
		float slopeY;
		if (map_->GetSlopeHeight(transform_.translate, slopeY)) {
			// エネミーの足元の高さ
			float playerBottom = transform_.translate.y + mapCollosion_.min.y;
			// エネミーの頭頂部の高さ
			float playerTop = transform_.translate.y + mapCollosion_.max.y;
			
			// スロープの高さとの距離を計算
			float distanceToSlope = playerBottom - slopeY;
			
			// スロープの表面付近にいるか、スロープより下にいる場合
			// かつ、エネミーの頭頂部がスロープ表面より上にある場合のみ吸着
			// プレイヤーと同じ閾値0.2fを使用
			if (distanceToSlope <= 0.2f && velocity_Y <= 0.0f && playerTop > slopeY) {
				// スロープに吸着
				transform_.translate.y = slopeY - mapCollosion_.min.y;
				velocity_Y = 0.0f;
				isGrounded_ = true;
				onSlope = true;
			}
		}
	}
	
	// 地面に着地した場合（下方向の速度がある場合のみ）
	if (isGrounded_ && velocity_Y <= 0.0f) {
		velocity_Y = 0.0f;
	}
	
	// 重力を適用（スロープ上でない場合、または上昇中の場合）
	if ((!isGrounded_ || velocity_Y > 0.0f) && !onSlope) {
		velocity_Y -= gravity_ * GetDeltaTime();
		transform_.translate.y += velocity_Y * GetDeltaTime();
	}
	
	// .xz平面でターゲット位置への方向ベクトルを計算（yは無視）
	Vector3 direction = {
		targetPosition_.x - transform_.translate.x,
		0.0f, // y成分は0にして水平移動のみ
		targetPosition_.z - transform_.translate.z
	};

	// .xz平面での距離を計算
	float distanceXZ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

	// ターゲットに到達していない場合のみ移動と回転
	if (distanceXZ > 0.01f) { // 微小な閾値で到達判定
		// .xz平面で正規化（yは0のまま）
		direction.x /= distanceXZ;
		direction.z /= distanceXZ;
		
		// 移動方向を保存（衝突判定で使用）
		moveDirection_ = direction;

		// ターゲット方向を向くように回転を計算
		// atan2(x,z)でY軸周りの回転角度を求める
		transform_.rotate.y = std::atan2(direction.x, direction.z);

		// 移動速度を適用
		Vector3 velocity = {
			direction.x * moveSpeed_ * GetDeltaTime(),
			0.0f, // y方向には移動しない
			direction.z * moveSpeed_ * GetDeltaTime()
		};

		// 位置を更新（yはそのまま）
		transform_.translate.x += velocity.x;
		transform_.translate.z += velocity.z;
	} else {
		moveDirection_ = { 0.0f, 0.0f, 0.0f };
	}
}

void Enemy::ResolveMapCollision() {
	if (!map_) return;

	// エネミーの現在位置からマップ座標を取得
	uint32_t centerX, centerY, centerZ;
	if (!map_->WorldToMap(transform_.translate, centerX, centerY, centerZ)) {
		// マップ範囲外の場合は何もしない
		return;
	}

	// チェック範囲を1に縮小（パフォーマンス向上）
	const int32_t checkRange = 1;
	
	// エネミーのAABBをワールド座標で取得
	Vector3 playerMin = mapCollosion_.GetMinWorld();
	Vector3 playerMax = mapCollosion_.GetMaxWorld();

	// スロープ上にいるかチェック（プレイヤーと同様の処理）
	float slopeY;
	bool isOnSlope = map_->GetSlopeHeight(transform_.translate, slopeY);
	float playerBottom = transform_.translate.y + mapCollosion_.min.y;
	float playerTop = transform_.translate.y + mapCollosion_.max.y;
	float distanceToSlope = playerBottom - slopeY;
	bool standingOnSlope = false;
	
	// スロープの上に立っている判定（プレイヤーと同じ閾値）
	// 下り方向への移動を考慮して許容範囲を広げる（-1.0f ~ 1.0f)
	if (isOnSlope && distanceToSlope >= -1.0f && distanceToSlope <= 1.0f && playerTop > slopeY) {
		standingOnSlope = true;
	}

	// スロープ上にいる場合は、エレベーター方式の上昇をスキップ
	// Move()関数でスロープに吸着する処理が行われるため
	if (standingOnSlope) {
		return;
	}

	// 衝突しているブロックの中で最も高い上面を見つける
	float highestBlockTop = -std::numeric_limits<float>::max();
	bool hasBlockCollision = false;
	bool hasSlopeCollision = false; // スロープ衝突フラグを追加
	float highestSlopeTop = -std::numeric_limits<float>::max(); // スロープの実際の高さ

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
				
				// スロープの処理
				bool isSlopeTile = (tileType == TileType::Slope || 
				                    tileType == TileType::Slope_PlusX || 
				                    tileType == TileType::Slope_MinusX || 
				                    tileType == TileType::Slope_PlusZ || 
				                    tileType == TileType::Slope_MinusZ);
				
				if (isSlopeTile) {
					Vector3 blockWorldPos = map_->MapToWorld(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
					
					// スロープブロック全体のAABB
					AABB slopeBlockAABB;
					slopeBlockAABB.center = blockWorldPos;
					slopeBlockAABB.min = { -7.5f, -5.0f, -7.5f };
					slopeBlockAABB.max = {  7.5f,  5.0f,  7.5f };
					
					// スロープブロックとの衝突判定
					if (Collision::IsHit(mapCollosion_, slopeBlockAABB)) {
						// スロープ表面の上にいるかチェック（プレイヤーと同じ処理）
						float playerSlopeHeight;
						bool playerIsOnSlopeSurface = false;
						if (map_->GetSlopeHeight(transform_.translate, playerSlopeHeight)) {
							float distToSurface = playerBottom - playerSlopeHeight;
							// 表面から十分に上にいる場合（スロープの上に立っている）
							// プレイヤーと同じ閾値-0.15f ~ 0.15fを使用
							if (distToSurface >= -0.15f && distToSurface <= 0.15f) {
								playerIsOnSlopeSurface = true;
							}
						}
						
						// スロープ表面の上にいない場合（側面衝突）のみエレベーター対象
						if (!playerIsOnSlopeSurface) {
							// スロープの場合、現在位置でのスロープ表面の高さを取得
							float currentSlopeHeight;
							if (map_->GetSlopeHeight(transform_.translate, currentSlopeHeight)) {
								// スロープ表面の高さを記録
								if (currentSlopeHeight > highestSlopeTop) {
									highestSlopeTop = currentSlopeHeight;
									hasSlopeCollision = true;
								}
							}
						}
					}
					continue;
				}

				// Normalブロックのチェック
				if (tileType != TileType::Normal) continue;

				// ブロックのAABBを取得
				Vector3 blockWorldPos = map_->MapToWorld(static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z));
				AABB blockAABB;
				blockAABB.center = blockWorldPos;
				blockAABB.min = { -7.5f, -5.0f, -7.5f };
				blockAABB.max = {  7.5f,  5.0f,  7.5f };

				// 衝突判定
				if (!Collision::IsHit(mapCollosion_, blockAABB)) continue;

				// 衝突している場合、押し出し量を計算
				Vector3 blockMin = blockAABB.GetMinWorld();
				Vector3 blockMax = blockAABB.GetMaxWorld();

				// 各軸での重なり量を計算
				float overlapX = (std::min)(playerMax.x - blockMin.x, blockMax.x - playerMin.x);
				float overlapY = (std::min)(playerMax.y - blockMin.y, blockMax.y - playerMin.y);
				float overlapZ = (std::min)(playerMax.z - blockMin.z, blockMax.z - playerMin.z);

				// 側面衝突かどうかを判定
				// エネミーの足元がブロックの上面より下にある場合は側面衝突
				float blockTop = blockAABB.GetMaxWorld().y;
				
				if (playerBottom < blockTop - 0.1f) {
					// 側面に衝突している -> エレベーター方式で上昇
					// 最も高いブロックの上面を記録
					if (blockTop > highestBlockTop) {
						highestBlockTop = blockTop;
						hasBlockCollision = true;
					}
				} else {
					// 上から乗っている場合は通常の処理
					// Y軸方向の押し出し（下方向へ落ちないように）
					if (overlapY > 0.0f && transform_.translate.y > blockWorldPos.y) {
						// ブロックの上に乗せる
						transform_.translate.y = blockTop - mapCollosion_.min.y;
						velocity_Y = 0.0f;
						isGrounded_ = true;
					}
				}
			}
		}
	}

	// ブロックに埋まっている場合はエレベーター方式で上昇
	// スロープ衝突とNormalブロック衝突の両方をチェック
	if (hasBlockCollision || hasSlopeCollision) {
		// 目標の高さを決定
		float targetY;
		
		// スロープ衝突がある場合は、スロープ表面の高さを優先
		// （スロープ表面より上には上昇しない）
		if (hasSlopeCollision) {
			targetY = highestSlopeTop - mapCollosion_.min.y;
			
			// Normalブロック衝突もある場合は、より低い方を選択
			if (hasBlockCollision) {
				float normalTargetY = highestBlockTop - mapCollosion_.min.y + 0.01f;
				targetY = (std::min)(targetY, normalTargetY);
			}
		} else {
			// Normalブロック衝突のみの場合
			targetY = highestBlockTop - mapCollosion_.min.y + 0.01f;
		}
		
		// 現在の高さとの差
		float heightDiff = targetY - transform_.translate.y;
		
		if (heightDiff > 0.0f) {
			// エレベーター方式：壁に向かって移動中はスムーズに上昇
			// 上昇速度を計算（移動している場合は速く、そうでない場合はゆっくり）
			float moveIntensity = std::sqrt(moveDirection_.x * moveDirection_.x + moveDirection_.z * moveDirection_.z);
			
			if (moveIntensity > 0.01f) {
				// 移動中は速く上昇
				float climbSpeed = wallClimbSpeed_ * GetDeltaTime();
				
				// 一気に目標地点まで上昇（ただし上昇速度の上限あり）
				float climbAmount = (std::min)(heightDiff, climbSpeed);
				transform_.translate.y += climbAmount;
				
				// 上昇中は重力をリセット
				velocity_Y = 0.0f;
			} else {
				// 静止中でも少しずつ上昇
				float climbAmount = (std::min)(heightDiff, wallClimbSpeed_ * 0.5f * GetDeltaTime());
				transform_.translate.y += climbAmount;
				velocity_Y = 0.0f;
			}
		}
		
		// AABBの中心も更新
		mapCollosion_.center = transform_.translate;
	}
}

bool Enemy::IsGroundedOnMap() {
	if (!map_) return false;

	// エネミーの足元の少し下の位置をチェック
	const float groundCheckDistance = 0.15f;
	
	// エネミーの足元のAABBを作成（少し下に拡張）
	AABB groundCheckAABB;
	groundCheckAABB.center = transform_.translate;
	groundCheckAABB.min = mapCollosion_.min;
	groundCheckAABB.max = mapCollosion_.max;
	// 足元を少し下に拡張
	groundCheckAABB.min.y -= groundCheckDistance;

	// エネミーの現在位置からマップ座標を取得
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
						// ブロックが足元にある場合、そのブロックの上面がエネミーの足元より少し下にあるかチェック
						float blockTop = blockAABB.GetMaxWorld().y;
						float playerBottom = mapCollosion_.GetMinWorld().y;
						
						// ブロックの上面がエネミーの足元付近にある場合のみ地面と判定
						float distance = playerBottom - blockTop;
						if (distance >= -0.01f && distance <= groundCheckDistance) {
							return true;
						}
					}
				}
				// スロープのチェック（プレイヤーと同じ処理）
				else if (tileType == TileType::Slope || 
				         tileType == TileType::Slope_PlusX || 
				         tileType == TileType::Slope_MinusX || 
				         tileType == TileType::Slope_PlusZ || 
				         tileType == TileType::Slope_MinusZ) {
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

void Enemy::PushAway(const Vector3& otherPosition, float otherRadius) {
	// 自分と他のEnemyとの方向ベクトルを計算（xz平面のみ）
	Vector3 direction = {
		transform_.translate.x - otherPosition.x,
		0.0f,
		transform_.translate.z - otherPosition.z
	};

	// .xz平面での距離を計算
	float distance = std::sqrt(direction.x * direction.x + direction.z * direction.z);

	// 重なり量を計算
	float radiusSum = collicionRadius_ + otherRadius;
	float overlap = radiusSum - distance;

	// 重なっている場合のみ押し出す
	if (overlap > 0.0f && distance > 0.01f) {
		// 正規化
		direction.x /= distance;
		direction.z /= distance;

		// 押し出す距離の半分ずつ移動（お互いに押し合う）
		float pushDistance = overlap * 0.5f;
		transform_.translate.x += direction.x * pushDistance;
		transform_.translate.z += direction.z * pushDistance;
	}
}