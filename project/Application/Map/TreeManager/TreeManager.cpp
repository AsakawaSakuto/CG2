#include "TreeManager.h"
#include "Utility/Random/Random.h"
#include "Utility/Collision/Collision.h"
#include <algorithm>
#include <random>

void TreeManager::Initialize(Map3D* map) {
	// 木の配列を予約
	trees_.reserve(treeNum_);
	
	// 木を生成
	SpawnTrees(map);
}

void TreeManager::Update() {
	// 現時点では木の更新処理なし
	// 必要に応じて追加可能（例：揺れアニメーション、破壊処理など）
}

void TreeManager::Draw(Camera& camera) {
	// 全ての木を描画
	for (auto& tree : trees_) {
		tree->Draw(camera);
	}
}

void TreeManager::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("TreeManager");
	
	ImGui::Text("=== Tree Status ===");
	ImGui::Text("Trees: %d / %d", static_cast<int>(trees_.size()), treeNum_);
	
	ImGui::Separator();
	ImGui::Text("Settings:");
	ImGui::DragFloat("Min Tree Distance", &minTreeDistance_, 0.1f, 0.5f, 20.0f);
	
	ImGui::End();
#endif
}

bool TreeManager::FindTopNormalBlock(Map3D* map, uint32_t x, uint32_t z, uint32_t& outY) const {
	// 上から下に向かって最初のNormalブロックを探す
	for (int32_t y = static_cast<int32_t>(map->GetHeight()) - 1; y >= 0; --y) {
		TileType type = map->GetTile(x, static_cast<uint32_t>(y), z);
		
		// Normalブロックを見つけた場合
		if (type == TileType::Normal) {
			// このNormalブロックの上にスロープがないかチェック
			if (y < static_cast<int32_t>(map->GetHeight()) - 1) {
				TileType topType = map->GetTile(x, static_cast<uint32_t>(y + 1), z);
				// 上にSlopeがある場合はスキップ（Slopeの中に配置されるのを防ぐ）
				if (topType == TileType::Slope || 
				    topType == TileType::Slope_PlusX || 
				    topType == TileType::Slope_MinusX || 
				    topType == TileType::Slope_PlusZ || 
				    topType == TileType::Slope_MinusZ) {
					continue;
				}
			}
			
			// これが最上面のNormalブロックであることを確認
			// この上に別のNormalブロックやスロープがないことを確認
			bool isTopMost = true;
			for (int32_t checkY = y + 1; checkY < static_cast<int32_t>(map->GetHeight()); ++checkY) {
				TileType checkType = map->GetTile(x, static_cast<uint32_t>(checkY), z);
				if (checkType != TileType::Empty) {
					isTopMost = false;
					break;
				}
			}
			
			if (isTopMost) {
				outY = static_cast<uint32_t>(y);
				return true;
			}
		}
	}
	return false;
}

bool TreeManager::IsPositionValid(const Vector3& pos, float minDistance) const {
	// 既存の木との距離をチェック
	for (const auto& tree : trees_) {
		Vector3 diff = pos - tree->GetPosition();
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		if (distanceSq < minDistance * minDistance) {
			return false;
		}
	}
	
	return true;
}

void TreeManager::SpawnTrees(Map3D* map) {
	// 木を生成するための候補位置をリストアップ
	std::vector<Vector3> candidatePositions;
	
	// マップ全体をスキャンして最上面のNormalブロックを探す
	for (uint32_t z = 0; z < map->GetDepth(); ++z) {
		for (uint32_t x = 0; x < map->GetWidth(); ++x) {
			uint32_t topY;
			if (FindTopNormalBlock(map, x, z, topY)) {
				// ブロックの中心座標を取得
				Vector3 blockCenter = map->MapToWorld(x, topY, z);
				
				// XZ方向に±5.0の範囲でランダムな位置を生成（ブロックサイズ15.0の半分より小さく）
				// より多くの候補を生成（10個に増やす）
				for (int i = 0; i < 10; ++i) {
					float offsetX = MyRand::Float(-5.0f, 5.0f);
					float offsetZ = MyRand::Float(-5.0f, 5.0f);
					
					Vector3 treePos = {
						blockCenter.x + offsetX,
						blockCenter.y + 5.0f,  // Y座標はCenter.y + 5.0f（ブロックの高さ10.0の半分）
						blockCenter.z + offsetZ
					};
					
					// スロープ上かチェック
					float slopeHeight;
					if (map->GetSlopeHeight(treePos, slopeHeight)) {
						// スロープ上の場合は候補から除外
						continue;
					}
					
					// 木の位置から周辺のブロックをチェックして、スロープが近くにないか確認
					uint32_t checkX, checkY, checkZ;
					if (map->WorldToMap(treePos, checkX, checkY, checkZ)) {
						// 上のブロックがスロープでないことを確認
						if (checkY + 1 < map->GetHeight()) {
							TileType aboveType = map->GetTile(checkX, checkY + 1, checkZ);
							if (aboveType == TileType::Slope || 
							    aboveType == TileType::Slope_PlusX || 
							    aboveType == TileType::Slope_MinusX || 
							    aboveType == TileType::Slope_PlusZ || 
							    aboveType == TileType::Slope_MinusZ) {
								// 上にスロープがある場合は除外
								continue;
							}
						}
					}
					
					candidatePositions.push_back(treePos);
				}
			}
		}
	}
	
	// 候補位置をシャッフル
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(candidatePositions.begin(), candidatePositions.end(), g);
	
	// 木を配置
	int treeCount = 0;
	
	for (size_t i = 0; i < candidatePositions.size() && treeCount < treeNum_; ++i) {
		const auto& pos = candidatePositions[i];
		
		// 他の木と十分離れているかチェック
		if (!IsPositionValid(pos, minTreeDistance_)) {
			continue;
		}
		
		// 木を生成
		auto tree = std::make_unique<Tree>();
		tree->Initialize(pos);
		trees_.push_back(std::move(tree));
		++treeCount;
	}
}

std::vector<Vector3> TreeManager::GetAllTreePositions() const {
	std::vector<Vector3> positions;
	
	// 全ての木の位置を追加
	for (const auto& tree : trees_) {
		positions.push_back(tree->GetPosition());
	}
	
	return positions;
}

void TreeManager::ResolvePlayerCollision(Vector3& playerPosition, const AABB& playerAABB) {
	// XZ軸のみの衝突判定用のAABBを作成（Y軸は無限に設定）
	AABB playerAABB_XZ = playerAABB;
	playerAABB_XZ.center = playerPosition;
	playerAABB_XZ.min.y = -10000.0f;  // Y軸を無視するために非常に広い範囲に設定
	playerAABB_XZ.max.y = 10000.0f;
	
	const int maxIterations = 4;  // 最大反復回数
	
	for (int iteration = 0; iteration < maxIterations; ++iteration) {
		bool hasCollision = false;
		Vector3 bestPushOut = { 0.0f, 0.0f, 0.0f };
		float minPenetration = (std::numeric_limits<float>::max)();
		
		// プレイヤーのAABBを更新
		playerAABB_XZ.center = playerPosition;
		Vector3 playerMin = playerAABB_XZ.GetMinWorld();
		Vector3 playerMax = playerAABB_XZ.GetMaxWorld();
		
		// 全ての木との衝突をチェック
		for (const auto& tree : trees_) {
			if (!tree->IsSpawned()) continue;
			
			// TreeのAABBを取得してXZ軸のみの判定用に変換
			AABB treeAABB = tree->GetAABBCollision();
			treeAABB.min.y = -10000.0f;  // Y軸を無視
			treeAABB.max.y = 10000.0f;
			
			// XZ軸のみの衝突判定
			if (!Collision::IsHit(playerAABB_XZ, treeAABB)) {
				continue;
			}
			
			hasCollision = true;
			
			// 木のAABBのワールド座標を取得
			Vector3 treeMin = treeAABB.GetMinWorld();
			Vector3 treeMax = treeAABB.GetMaxWorld();
			
			// XとZ軸での重なり量を計算
			float overlapX = (std::min)(playerMax.x - treeMin.x, treeMax.x - playerMin.x);
			float overlapZ = (std::min)(playerMax.z - treeMin.z, treeMax.z - playerMin.z);
			
			// X軸とZ軸で押し出し量が小さい方を選択
			if (overlapX < overlapZ) {
				if (overlapX < minPenetration) {
					minPenetration = overlapX;
					// プレイヤーが木の左右どちら側にいるか判定
					if (playerPosition.x < treeAABB.center.x) {
						bestPushOut = { -overlapX, 0.0f, 0.0f };  // 左に押し出し
					} else {
						bestPushOut = { overlapX, 0.0f, 0.0f };   // 右に押し出し
					}
				}
			} else {
				if (overlapZ < minPenetration) {
					minPenetration = overlapZ;
					// プレイヤーが木の前後どちら側にいるか判定
					if (playerPosition.z < treeAABB.center.z) {
						bestPushOut = { 0.0f, 0.0f, -overlapZ };  // 手前に押し出し
					} else {
						bestPushOut = { 0.0f, 0.0f, overlapZ };   // 奥に押し出し
					}
				}
			}
		}
		
		// 衝突がない場合は終了
		if (!hasCollision) {
			break;
		}
		
		// 押し出しを適用（XZ軸のみ）
		playerPosition.x += bestPushOut.x;
		playerPosition.z += bestPushOut.z;
	}
}

void TreeManager::SetOccludersTransparent(const Vector3& cameraPos, const Vector3& playerPos) {
	// カメラからプレイヤーへの方向ベクトル
	Vector3 direction = playerPos - cameraPos;
	float distance = direction.Length();
	
	if (distance < 0.01f) return;
	
	direction = direction / distance; // 正規化
	
	const float checkRadius = 10.0f; // チェック半径
	
	// 全ての木をチェック
	for (auto& tree : trees_) {
		if (!tree->IsSpawned()) continue;
		
		Vector3 treePos = tree->GetPosition();
		
		// 木の位置がカメラ-プレイヤー線上付近にあるかチェック
		Vector3 toTree = treePos - cameraPos;
		float projectionLength = Vector3::Dot(toTree, direction);
		
		// プレイヤーより手前にある木のみ処理
		if (projectionLength < 0.0f || projectionLength > distance) {
			continue;
		}
		
		Vector3 closestPoint = cameraPos + direction * projectionLength;
		float distanceToLine = (treePos - closestPoint).Length();
		
		// 線に近い木を半透明化
		if (distanceToLine < checkRadius) {
			tree->SetTransparent(true, 0.3f);
		}
	}
}

void TreeManager::ResetAllTransparency() {
	for (auto& tree : trees_) {
		if (tree->IsSpawned()) {
			tree->SetTransparent(false);
		}
	}
}
