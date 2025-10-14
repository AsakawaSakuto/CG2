#pragma once
#include "EmitterSpfere.h"
#include <string>
#include "externals/imgui/imgui.h"

class EmitterStateLoader {
public:
	// Jsonファイルから読み込む関数
    static EmitterState Load(const std::string& filePath);

	// Jsonファイルに保存する関数
    static void Save(const std::string& filePath, const EmitterState& state);

    // cppのある階層にJsonファイルを出力する関数
    static void SaveToCurrentDir(const EmitterState& state, const std::string& ImGuiName);

    static bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0);
};
