#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Map {
public:
	void Initialize();

	void Update();

	void Draw();

	// CCSVファイルからマップチップデータの読み込み格納
	void LoadMapCSV(const std::string& filename);

private:
	// マップデータ
	std::vector<std::vector<int>> mapData_;
};
