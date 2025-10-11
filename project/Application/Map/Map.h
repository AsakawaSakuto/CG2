#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

enum class TileType {
	EMPTY = 0,
	THORN = 1,
	BLOCK = 2,
};

class Map {
public:
	void Initialize();

	void Update();

	void Draw();

	// CCSVファイルからマップチップデータの読み込み格納
	std::vector<std::vector<int>> LoadMapCSV(const std::string& filename);

	// タイルタイプ取得
	int GetMapData(int row, int col) const;

	// 行数を取得
	int GetRowCount() const { return static_cast<int>(mapData1_.size()); }

	// 列数を取得
	int GetColumnCount() const { return mapData1_.empty() ? 0 : static_cast<int>(mapData1_[0].size()); }

	// タイルタイプ取得
	int GetMapData2(int row, int col) const;

	// 行数を取得
	int GetRowCount2() const { return static_cast<int>(mapData2_.size()); }

	// 列数を取得
	int GetColumnCount2() const { return mapData2_.empty() ? 0 : static_cast<int>(mapData2_[0].size()); }

private:
	// マップデータ
	std::vector<std::vector<int>> mapData1_;
	std::vector<std::vector<int>> mapData2_;

	// マップチップのサイズ
	const int tileSize_ = 1;

	// マップの幅と高さ
	const int mapWidth_ = 20;
	const int mapHeight_ = 20;
};
