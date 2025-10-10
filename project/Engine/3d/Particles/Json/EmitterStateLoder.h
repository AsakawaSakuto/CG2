#pragma once
#include "EmitterSpfere.h"
#include <string>
#include "externals/imgui/imgui.h"

class EmitterStateLoader {
public:
	// Jsonファイルから読み込む関数
    static EmitterSphere Load(const std::string& filePath);

	// Jsonファイルに保存する関数
    static void Save(const std::string& filePath, const EmitterSphere& state);

    // cppのある階層にJsonファイルを出力する関数
    static void SaveToCurrentDir(const EmitterSphere& state, const std::string& ImGuiName);

    static bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0);
};
