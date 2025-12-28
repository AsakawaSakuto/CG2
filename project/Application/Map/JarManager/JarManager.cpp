#include "JarManager.h"
#include "Utility/Random/Random.h"
#include "Utility/Collision/Collision.h"
#include <algorithm>
#include <random>

void JarManager::Initialize(Map3D* map) {
	// 壺の配列を予約
	minJars_.reserve(minJarNum_);
	maxJars_.reserve(maxJarNum_);
	
	// 壺を生成
	SpawnJars(map);
	
	// デバッグ: 実際に生成された数を出力
	int actualMinJars = static_cast<int>(minJars_.size());
	int actualMaxJars = static_cast<int>(maxJars_.size());
}

void JarManager::Update() {
	// 死んでいる壺を削除
	minJars_.erase(
		std::remove_if(minJars_.begin(), minJars_.end(),
			[](const std::unique_ptr<MinJar>& jar) { return !jar->IsAlive(); }),
		minJars_.end()
	);
	
	maxJars_.erase(
		std::remove_if(maxJars_.begin(), maxJars_.end(),
			[](const std::unique_ptr<MaxJar>& jar) { return !jar->IsAlive(); }),
		maxJars_.end()
	);
}

void JarManager::Draw(Camera& camera) {
	// 全てのMinJarを描画
	for (auto& jar : minJars_) {
		if (jar->IsAlive()) {
			jar->Draw(camera);
		}
	}
	
	// 全てのMaxJarを描画
	for (auto& jar : maxJars_) {
		if (jar->IsAlive()) {
			jar->Draw(camera);
		}
	}
}

void JarManager::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("JarManager");
	
	ImGui::Text("=== Jar Status ===");
	ImGui::Text("MinJar: %d / %d", static_cast<int>(minJars_.size()), minJarNum_);
	ImGui::Text("MaxJar: %d / %d", static_cast<int>(maxJars_.size()), maxJarNum_);
	ImGui::Text("Total: %d / %d", 
		static_cast<int>(minJars_.size() + maxJars_.size()), 
		minJarNum_ + maxJarNum_);
	
	ImGui::Separator();
	ImGui::Text("Settings:");
	ImGui::DragFloat("Min Jar Distance", &minJarDistance_, 0.1f, 0.5f, 20.0f);
	
	ImGui::End();
#endif
}

int JarManager::BreakJar(const AABB& attackAABB, JarType& outJarType) {
	// MinJarとの衝突チェック
	for (auto& jar : minJars_) {
		if (jar->IsAlive()) {
			if (Collision::IsHit(attackAABB, jar->GetAABBCollision())) {
				outJarType = jar->GetJarType();
				return jar->Break();
			}
		}
	}
	
	// MaxJarとの衝突チェック
	for (auto& jar : maxJars_) {
		if (jar->IsAlive()) {
			if (Collision::IsHit(attackAABB, jar->GetAABBCollision())) {
				outJarType = jar->GetJarType();
				return jar->Break();
			}
		}
	}
	
	return 0;
}

bool JarManager::FindTopNormalBlock(Map3D* map, uint32_t x, uint32_t z, uint32_t& outY) const {
	// 上から下に向かって最初のNormalブロックを探す
	for (int32_t y = static_cast<int32_t>(map->GetHeight()) - 1; y >= 0; --y) {
		TileType type = map->GetTile(x, static_cast<uint32_t>(y), z);
		if (type == TileType::Normal) {
			outY = static_cast<uint32_t>(y);
			return true;
		}
	}
	return false;
}

bool JarManager::IsPositionValid(const Vector3& pos, float minDistance) const {
	// 既存のMinJarとの距離をチェック
	for (const auto& jar : minJars_) {
		Vector3 diff = pos - jar->GetPosition();
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		if (distanceSq < minDistance * minDistance) {
			return false;
		}
	}
	
	// 既存のMaxJarとの距離をチェック
	for (const auto& jar : maxJars_) {
		Vector3 diff = pos - jar->GetPosition();
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		if (distanceSq < minDistance * minDistance) {
			return false;
		}
	}
	
	return true;
}

void JarManager::SpawnJars(Map3D* map) {
	// 壺を生成するための候補位置をリストアップ
	std::vector<Vector3> candidatePositions;
	
	// マップ全体をスキャンして最上面のNormalブロックを探す
	for (uint32_t z = 0; z < map->GetDepth(); ++z) {
		for (uint32_t x = 0; x < map->GetWidth(); ++x) {
			uint32_t topY;
			if (FindTopNormalBlock(map, x, z, topY)) {
				// ブロックの中心座標を取得
				Vector3 blockCenter = map->MapToWorld(x, topY, z);
				
				// XZ方向に±4.5の範囲でランダムな位置を生成
				// より多くの候補を生成（8個に増やす）
				for (int i = 0; i < 8; ++i) {
					float offsetX = MyRand::Float(-4.5f, 4.5f);
					float offsetZ = MyRand::Float(-4.5f, 4.5f);
					
					Vector3 jarPos = {
						blockCenter.x + offsetX,
						blockCenter.y + 2.5f,  // Y座標はCenter.y + 2.5f
						blockCenter.z + offsetZ
					};
					
					candidatePositions.push_back(jarPos);
				}
			}
		}
	}
	
	// 候補位置をシャッフル
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(candidatePositions.begin(), candidatePositions.end(), g);
	
	// MinJarとMaxJarを交互に生成する（より均等に配置）
	int minJarCount = 0;
	int maxJarCount = 0;
	int totalTarget = minJarNum_ + maxJarNum_;
	
	for (size_t i = 0; i < candidatePositions.size() && (minJarCount < minJarNum_ || maxJarCount < maxJarNum_); ++i) {
		const auto& pos = candidatePositions[i];
		
		// 他の壺と十分離れているかチェック
		if (!IsPositionValid(pos, minJarDistance_)) {
			continue;
		}
		
		// MinJarとMaxJarの比率に応じて配置を決定
		bool spawnMinJar = false;
		if (minJarCount < minJarNum_ && maxJarCount >= maxJarNum_) {
			// MaxJarは上限に達したのでMinJarのみ
			spawnMinJar = true;
		} else if (maxJarCount < maxJarNum_ && minJarCount >= minJarNum_) {
			// MinJarは上限に達したのでMaxJarのみ
			spawnMinJar = false;
		} else {
			// 両方まだ配置可能な場合、目標比率に応じて決定
			float minJarRatio = static_cast<float>(minJarNum_) / static_cast<float>(totalTarget);
			float currentMinJarRatio = (minJarCount + maxJarCount > 0) 
				? static_cast<float>(minJarCount) / static_cast<float>(minJarCount + maxJarCount)
				: 0.0f;
			
			spawnMinJar = currentMinJarRatio < minJarRatio;
		}
		
		if (spawnMinJar && minJarCount < minJarNum_) {
			auto jar = std::make_unique<MinJar>();
			jar->Initialize(pos);
			minJars_.push_back(std::move(jar));
			++minJarCount;
		} else if (!spawnMinJar && maxJarCount < maxJarNum_) {
			auto jar = std::make_unique<MaxJar>();
			jar->Initialize(pos);
			maxJars_.push_back(std::move(jar));
			++maxJarCount;
		}
	}
}
