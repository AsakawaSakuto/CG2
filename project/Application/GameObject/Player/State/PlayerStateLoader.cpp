#include "PlayerStateLoader.h"
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

PlayerState PlayerStateLoader::Load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open PlayerStatus file: " + filePath);
    }

    json data;
    file >> data;

    PlayerState status;
    status.maxSpeed = data.value("maxSpeed", 6.0f);
    status.cameraOffset = data.value("cameraOffsetUp", 4.0f);
    status.bulletGaugeMax = data.value("bulletGaugeMax", 5);
    status.stunDuration = data.value("stunDuration", 1.0f);

    return status;
}

void PlayerStateLoader::Save(const std::string& filePath, const PlayerState& state) {
    json data = {
        {"maxSpeed", state.maxSpeed},
        {"cameraOffset", state.cameraOffset},
        {"bulletGaugeMax", state.bulletGaugeMax},
        {"stunDuration", state.stunDuration}
    };

    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to save PlayerState file: " + filePath);
    }

    file << std::setw(4) << data;
}