#include "Map.h"

void Map::Initialize() {
	// マップチップデータ読み込み格納
	mapData1_ = LoadMapCSV("resources/map/map5.csv");
}

void Map::Update() {}

void Map::Draw() {}

std::vector<std::vector<int>> Map::LoadMapCSV(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open map file: " + filename);
    }

    // ファイル全体をメモリに読み込む
    std::string buffer((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    std::vector<std::vector<int>> mapData;
    mapData.reserve(403); // 予想行数を入れておくといい

    std::vector<int> row;
    row.reserve(14); // 予想列数

    const char* ptr = buffer.data();
    const char* end = ptr + buffer.size();
    int value = 0;
    bool inNumber = false;
    bool negative = false;

    while (ptr < end) {
        char c = *ptr++;
        if (c == ',' || c == '\n' || c == '\r') {
            if (inNumber) {
                row.push_back(negative ? -value : value);
                value = 0;
                inNumber = false;
                negative = false;
            }
            if (c == '\n') {
                if (!row.empty()) {
                    mapData.push_back(std::move(row));
                    row.clear();
                    row.reserve(256);
                }
            }
        } else if (c == '-') {
            negative = true;
        } else if (c >= '0' && c <= '9') {
            inNumber = true;
            value = value * 10 + (c - '0');
        }
    }

    // 最後の行の処理
    if (inNumber) {
        row.push_back(negative ? -value : value);
    }
    if (!row.empty()) {
        mapData.push_back(std::move(row));
    }

    return mapData;
}

int Map::GetMapData(int row, int col) const {
	if (row < 0 || row >= GetRowCount() || col < 0 || col >= GetColumnCount()) {
		return static_cast<int>(TileType::EMPTY); // 範囲外は空タイル
	}
	return mapData1_[row][col];
}
