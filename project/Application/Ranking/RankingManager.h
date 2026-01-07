#pragma once
#include <string>
#include <array>
#include <memory>
#include "Utility/FileFormat/Binary/BinaryManager.h"

/// <summary>
/// ランキング管理クラス
/// 敵を倒した数を上位3位まで保存・読み込みする
/// </summary>
class RankingManager {
public:
	static const int kMaxRankings = 3;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	RankingManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RankingManager() = default;

	/// <summary>
	/// ランキングデータを読み込む
	/// </summary>
	void Load();

	/// <summary>
	/// ランキングデータを保存する
	/// </summary>
	void Save();

	/// <summary>
	/// 新しいスコアを登録する
	/// ランキングに入る場合は自動的にソートされる
	/// </summary>
	/// <param name="killCount">敵を倒した数</param>
	/// <returns>ランキングに入った場合の順位（1-3）、入らなかった場合は0</returns>
	int RegisterScore(int killCount);

	/// <summary>
	/// 指定順位のスコアを取得
	/// </summary>
	/// <param name="rank">順位（1-3）</param>
	/// <returns>スコア（敵を倒した数）</returns>
	int GetScore(int rank) const;

	/// <summary>
	/// 1位のスコアを取得
	/// </summary>
	int GetFirst() const { return rankings_[0]; }

	/// <summary>
	/// 2位のスコアを取得
	/// </summary>
	int GetSecond() const { return rankings_[1]; }

	/// <summary>
	/// 3位のスコアを取得
	/// </summary>
	int GetThird() const { return rankings_[2]; }

private:
	/// <summary>
	/// ランキングをソートする（降順）
	/// </summary>
	void SortRankings();

private:
	std::array<int, kMaxRankings> rankings_;
	std::unique_ptr<BinaryManager> binaryManager_;
	std::string fileName_ = "ranking";
};
