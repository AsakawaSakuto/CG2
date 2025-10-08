#pragma once
#include "PlayerState.h"
#include <string>

class PlayerStateLoader {
public:
    static PlayerState Load(const std::string& filePath);
    static void Save(const std::string& filePath, const PlayerState& state);
};