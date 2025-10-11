#include "Map.h"

void Map::Initialize() {
	// マップチップデータ読み込み格納
	mapData1_ = LoadMapCSV("resources/map/map5.csv");
	mapData2_ = LoadMapCSV("resources/map/map4.csv");
}

void Map::Update() {}

void Map::Draw() {
	//for (int y = 0; y < mapData_.size(); ++y) {
	//	for (int x = 0; x < mapData_[y].size(); ++x) {
	//		int tile = mapData_[y][x];

	//		// タイルごとの描画処理
	//		switch (static_cast<TileType>(tile)) {
	//		case TileType::EMPTY:
	//			// 空

	//			break;
	//		case TileType::BLOCK:
	//			// ブロックの描画処理

	//			break;
	//		default:

	//			break;
	//		}
	//	}
	//}
}

std::vector<std::vector<int>> Map::LoadMapCSV(const std::string& filename) {
	std::vector<std::vector<int>> mapData;
	std::ifstream file(filename);
	std::string line;

	while (std::getline(file, line)) {
		std::vector<int> row;
		std::stringstream ss(line);
		std::string cell;

		while (std::getline(ss, cell, ',')) {
			row.push_back(std::stoi(cell));
		}

		mapData.push_back(row);
	}

	return mapData;
}

int Map::GetMapData(int row, int col) const {
	if (row < 0 || row >= GetRowCount() || col < 0 || col >= GetColumnCount()) {
		return static_cast<int>(TileType::EMPTY); // 範囲外は空タイル
	}
	return mapData1_[row][col];
}

int Map::GetMapData2(int row, int col) const {
	if (row < 0 || row >= GetRowCount2() || col < 0 || col >= GetColumnCount2()) {
		return static_cast<int>(TileType::EMPTY); // 範囲外は空タイル
	}
	return mapData2_[row][col];
}
