#include "ChestManager.h"
#include "Map/JarManager/JarManager.h"
#include "Utility/Random/Random.h"
#include "Utility/Collision/Collision.h"
#include <algorithm>
#include <random>

void ChestManager::Initialize(Map3D* map, JarManager* jarManager) {
	// 宝箱の配列を予約
	paidChests_.reserve(paidChestNum_);
	freeChests_.reserve(freeChestNum_);
	
	// JarManagerから壺の位置リストを取得
	std::vector<Vector3> jarPositions;
	if (jarManager) {
		jarPositions = jarManager->GetAllJarPositions();
	}
	
	// 宝箱を生成
	SpawnChests(map, jarPositions);
}

void ChestManager::Update() {
	// 開かれた宝箱を削除
	paidChests_.erase(
		std::remove_if(paidChests_.begin(), paidChests_.end(),
			[](const std::unique_ptr<PaidChest>& chest) { return !chest->IsAlive(); }),
		paidChests_.end()
	);
	
	freeChests_.erase(
		std::remove_if(freeChests_.begin(), freeChests_.end(),
			[](const std::unique_ptr<FreeChest>& chest) { return !chest->IsAlive(); }),
		freeChests_.end()
	);
}

void ChestManager::Draw(Camera& camera) {
	// 全てのPaidChestを描画
	for (auto& chest : paidChests_) {
		if (chest->IsAlive()) {
			chest->Draw(camera);
		}
	}
	
	// 全てのFreeChestを描画
	for (auto& chest : freeChests_) {
		if (chest->IsAlive()) {
			chest->Draw(camera);
		}
	}
}

void ChestManager::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("ChestManager");
	
	ImGui::Text("=== Chest Status ===");
	ImGui::Text("PaidChest: %d / %d", static_cast<int>(paidChests_.size()), paidChestNum_);
	ImGui::Text("FreeChest: %d / %d", static_cast<int>(freeChests_.size()), freeChestNum_);
	ImGui::Text("Total: %d / %d", 
		static_cast<int>(paidChests_.size() + freeChests_.size()), 
		paidChestNum_ + freeChestNum_);
	
	ImGui::Separator();
	ImGui::Text("Settings:");
	ImGui::DragFloat("Min Chest Distance", &minChestDistance_, 0.1f, 0.5f, 20.0f);
	ImGui::DragFloat("Min Chest-Jar Distance", &minChestJarDistance_, 0.1f, 0.5f, 20.0f);
	
	ImGui::End();
#endif
}

bool ChestManager::CheckChestCollision(const AABB& interactAABB, bool& outIsPaidChest, int& outOpenAmount) {
	// PaidChestとの衝突チェック
	for (auto& chest : paidChests_) {
		if (chest->IsAlive()) {
			if (Collision::IsHit(interactAABB, chest->GetAABBCollision())) {
				outIsPaidChest = true;
				outOpenAmount = openAmount_;
				return true;
			}
		}
	}
	
	// FreeChestとの衝突チェック
	for (auto& chest : freeChests_) {
		if (chest->IsAlive()) {
			if (Collision::IsHit(interactAABB, chest->GetAABBCollision())) {
				outIsPaidChest = false;
				outOpenAmount = 0;
				return true;
			}
		}
	}
	
	return false;
}

bool ChestManager::OpenChest(const AABB& interactAABB, bool needMoney) {
	if (needMoney) {
		// PaidChestを開ける
		for (auto& chest : paidChests_) {
			if (chest->IsAlive()) {
				if (Collision::IsHit(interactAABB, chest->GetAABBCollision())) {
					chest->Open();
					openAmount_ += addOpenAmount_; // 次回以降の開封コストを増加
					return true;
				}
			}
		}
	} else {
		// FreeChestを開ける
		for (auto& chest : freeChests_) {
			if (chest->IsAlive()) {
				if (Collision::IsHit(interactAABB, chest->GetAABBCollision())) {
					chest->Open();
					return true;
				}
			}
		}
	}
	
	return false;
}

bool ChestManager::FindTopNormalBlock(Map3D* map, uint32_t x, uint32_t z, uint32_t& outY) const {
	// 上から下に向かって最初の適切なNormalブロックを探す
	for (int32_t y = static_cast<int32_t>(map->GetHeight()) - 1; y >= 0; --y) {
		TileType type = map->GetTile(x, static_cast<uint32_t>(y), z);
		
		// Normalブロックのみを対象とする（Slope系は除外）
		if (type == TileType::Normal) {
			// このブロックの上（y+1）を確認
			if (y < static_cast<int32_t>(map->GetHeight()) - 1) {
				TileType aboveType = map->GetTile(x, static_cast<uint32_t>(y + 1), z);
				
				// 上にブロックがある場合はスキップ（最上面ではない）
				if (aboveType != TileType::Empty) {
					continue;
				}
			}
			
			// この位置が最上面のNormalブロックなので採用
			outY = static_cast<uint32_t>(y);
			return true;
		}
		
		// スロープブロックの場合はスキップ（スロープ上には配置しない）
		if (type == TileType::Slope || 
		    type == TileType::Slope_PlusX || 
		    type == TileType::Slope_MinusX || 
		    type == TileType::Slope_PlusZ || 
		    type == TileType::Slope_MinusZ) {
			continue;
		}
	}
	return false;
}

bool ChestManager::IsPositionValid(const Vector3& pos, float minDistance, const std::vector<Vector3>& existingPositions) const {
	// 既存のPaidChestとの距離をチェック
	for (const auto& chest : paidChests_) {
		Vector3 diff = pos - chest->GetPosition();
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		if (distanceSq < minDistance * minDistance) {
			return false;
		}
	}
	
	// 既存のFreeChestとの距離をチェック
	for (const auto& chest : freeChests_) {
		Vector3 diff = pos - chest->GetPosition();
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		if (distanceSq < minDistance * minDistance) {
			return false;
		}
	}
	
	// 壺の位置との距離をチェック
	for (const auto& jarPos : existingPositions) {
		Vector3 diff = pos - jarPos;
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		if (distanceSq < minChestJarDistance_ * minChestJarDistance_) {
			return false;
		}
	}
	
	return true;
}

void ChestManager::SpawnChests(Map3D* map, const std::vector<Vector3>& jarPositions) {
	// 宝箱を生成するための候補位置をリストアップ
	std::vector<Vector3> candidatePositions;
	
	// マップ全体をスキャンして最上面のNormalブロックを探す
	for (uint32_t z = 0; z < map->GetDepth(); ++z) {
		for (uint32_t x = 0; x < map->GetWidth(); ++x) {
			uint32_t topY;
			if (FindTopNormalBlock(map, x, z, topY)) {
				// ブロックの中心座標を取得
				Vector3 blockCenter = map->MapToWorld(x, topY, z);
				
				// XZ方向に±7.0の範囲でランダムな位置を生成（ブロックサイズ15.0の半分）
				// 各ブロックから複数の候補を生成
				for (int i = 0; i < 6; ++i) {
					float offsetX = MyRand::Float(-7.0f, 7.0f);
					float offsetZ = MyRand::Float(-7.0f, 7.0f);
					
					Vector3 chestPos = {
						blockCenter.x + offsetX,
						blockCenter.y + 5.0f,  // Y座標はCenter.y + 5.0f（ブロックの高さ10.0の半分）
						blockCenter.z + offsetZ
					};
					
					candidatePositions.push_back(chestPos);
				}
			}
		}
	}
	
	// 候補位置をシャッフル
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(candidatePositions.begin(), candidatePositions.end(), g);
	
	// PaidChestとFreeChestを配置
	int paidChestCount = 0;
	int freeChestCount = 0;
	int totalTarget = paidChestNum_ + freeChestNum_;
	
	for (size_t i = 0; i < candidatePositions.size() && (paidChestCount < paidChestNum_ || freeChestCount < freeChestNum_); ++i) {
		const auto& pos = candidatePositions[i];
		
		// 他の宝箱や壺と十分離れているかチェック
		if (!IsPositionValid(pos, minChestDistance_, jarPositions)) {
			continue;
		}
		
		// PaidChestとFreeChestの比率に応じて配置を決定
		bool spawnPaidChest = false;
		if (paidChestCount < paidChestNum_ && freeChestCount >= freeChestNum_) {
			// FreeChestは上限に達したのでPaidChestのみ
			spawnPaidChest = true;
		} else if (freeChestCount < freeChestNum_ && paidChestCount >= paidChestNum_) {
			// PaidChestは上限に達したのでFreeChestのみ
			spawnPaidChest = false;
		} else {
			// 両方まだ配置可能な場合、目標比率に応じて決定
			float paidChestRatio = static_cast<float>(paidChestNum_) / static_cast<float>(totalTarget);
			float currentPaidChestRatio = (paidChestCount + freeChestCount > 0) 
				? static_cast<float>(paidChestCount) / static_cast<float>(paidChestCount + freeChestCount)
				: 0.0f;
			
			spawnPaidChest = currentPaidChestRatio < paidChestRatio;
		}
		
		if (spawnPaidChest && paidChestCount < paidChestNum_) {
			auto chest = std::make_unique<PaidChest>();
			chest->Initialize(pos);
			paidChests_.push_back(std::move(chest));
			++paidChestCount;
		} else if (!spawnPaidChest && freeChestCount < freeChestNum_) {
			auto chest = std::make_unique<FreeChest>();
			chest->Initialize(pos);
			freeChests_.push_back(std::move(chest));
			++freeChestCount;
		}
	}
}
