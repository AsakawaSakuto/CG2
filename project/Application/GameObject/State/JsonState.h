#pragma once
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

class JsonState {
public:
	template<typename T> static T Load(const std::string& filePath) {
		std::ifstream file(filePath);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filePath);
		}

		json data;
		file >> data;
		return data.get<T>();
	}

	template<typename T> static void Save(const std::string& filePath, const T& state) {
		json data = state;

		std::ofstream file(filePath);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to save file: " + filePath);
		}

		file << std::setw(4) << data;
	}
};
