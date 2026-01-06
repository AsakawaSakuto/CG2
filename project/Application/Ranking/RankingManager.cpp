#include "RankingManager.h"
#include <algorithm>
#include <filesystem>

RankingManager::RankingManager() {
	// ランキングを0で初期化
	rankings_.fill(0);
	
	// BinaryManagerを初期化
	binaryManager_ = std::make_unique<BinaryManager>();
	binaryManager_->SetBasePath("resources/Data/Binary/");
}

void RankingManager::Load() {
	// ファイルからランキングを読み込む
	auto values = binaryManager_->Read(fileName_);
	
	if (values.empty()) {
		// ファイルが存在しない場合は初期値のまま
		rankings_.fill(0);
		return;
	}
	
	// 読み込んだ値を順番に取得
	for (size_t i = 0; i < kMaxRankings && i < values.size(); ++i) {
		rankings_[i] = BinaryManager::Reverse<int>(values[i]);
	}
}

void RankingManager::Save() {
	// ランキングをソートしてから保存
	SortRankings();
	
	// ファイルに書き込む
	for (int i = 0; i < kMaxRankings; ++i) {
		binaryManager_->RegistOutput(rankings_[i], "rank" + std::to_string(i + 1));
	}
	
	binaryManager_->Write(fileName_);
}

int RankingManager::RegisterScore(int killCount) {
	// スコアが0以下の場合は登録しない
	if (killCount <= 0) {
		return 0;
	}
	
	// 3位のスコアより低い場合はランク外
	if (killCount <= rankings_[kMaxRankings - 1]) {
		return 0;
	}
	
	// 新しいスコアをランキングに挿入
	// 最下位のスコアを新しいスコアで置き換え
	rankings_[kMaxRankings - 1] = killCount;
	
	// ソートして順位を決定
	SortRankings();
	
	// 登録したスコアの順位を返す
	for (int i = 0; i < kMaxRankings; ++i) {
		if (rankings_[i] == killCount) {
			// 保存
			Save();
			return i + 1;
		}
	}
	
	return 0;
}

int RankingManager::GetScore(int rank) const {
	if (rank < 1 || rank > kMaxRankings) {
		return 0;
	}
	return rankings_[rank - 1];
}

void RankingManager::SortRankings() {
	// 降順にソート（1位が最も大きい値）
	std::sort(rankings_.begin(), rankings_.end(), std::greater<int>());
}
