#include "Map.h"

void Map::Initialize() { 
	// マップチップデータ読み込み格納
	//LoadMapCSV(); 
}

void Map::Update()
{

}

void Map::Draw()
{

}

void Map::LoadMapCSV(const std::string& filename) { 
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

	mapData_ = mapData;
}
