#include "BinaryManager.h"
#include <fstream>

BinaryManager::BinaryManager() {
	input = std::make_unique<BinaryInput>();
	output = std::make_unique<BinaryOutput>();
}

BinaryManager::~BinaryManager() {
}

void BinaryManager::Write(std::string fileName) {
	// ファイル名に.bin拡張子がない場合は追加
	if (fileName.size() < 4 || fileName.substr(fileName.size() - 4) != ".bin") {
		fileName += ".bin";
	}

	std::ofstream file(basePath + fileName, std::ios::binary);

	if (!file.is_open()) {
		return;
	}

	for (auto v : values) {
		output->WriteBinary(file, v.get());
	}

	values.clear();

	file.close();
}

std::vector<std::shared_ptr<ValueBase>> BinaryManager::Read(std::string fileName) {
	// ファイル名に.bin拡張子がない場合は追加
	if (fileName.size() < 4 || fileName.substr(fileName.size() - 4) != ".bin") {
		fileName += ".bin";
	}

	std::ifstream file(basePath + fileName, std::ios::binary);
	if (!file.is_open()) {
		return {};
	}

	std::vector<std::shared_ptr<ValueBase>> ans;

	while(file.peek() != EOF) {
		auto val = input->ReadBinary(file);
		if (!val) {
			break;
		}
		ans.push_back(val);
	}

	file.close();

	return ans;
}
