#include "EmitterStateLoder.h"
#include <fstream>
#include <filesystem>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

// --- Vector2 / Vector3 をJSON対応にする ---
namespace nlohmann {
    template <>
    struct adl_serializer<Vector2> {
        static void to_json(json& j, const Vector2& v) {
            j = json::array({ v.x, v.y });
        }
        static void from_json(const json& j, Vector2& v) {
            v.x = j.at(0).get<float>();
            v.y = j.at(1).get<float>();
        }
    };

    template <>
    struct adl_serializer<Vector3> {
        static void to_json(json& j, const Vector3& v) {
            j = json::array({ v.x, v.y, v.z });
        }
        static void from_json(const json& j, Vector3& v) {
            v.x = j.at(0).get<float>();
            v.y = j.at(1).get<float>();
            v.z = j.at(2).get<float>();
        }
    };
}

// =============================================================
//                        Load
// =============================================================
EmitterSphere EmitterStateLoader::Load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open EmitterStatus file: " + filePath);
    }

    json data;
    file >> data;

    EmitterSphere state;

    // --- 基本設定 ---
    state.translate = data.value("translate", Vector3{ 0.0f, 0.0f, 0.0f });
    state.radius = data.value("radius", 1.0f);

    // --- 放出制御 ---
    state.useEmitter = data.value("useEmitter", 1u);
    state.emit = data.value("emit", 0u);
    state.count = data.value("count", 1u);
    state.kMaxParticle = data.value("kMaxParticle", 0u);

    // --- 頻度 ---
    state.frequency = data.value("frequency", 1.0f);
    state.frequencyTime = data.value("frequencyTime", 0.0f);

    // --- スケール設定 ---
    state.startScale = data.value("startScale", Vector2{ 1.0f, 1.0f });
    state.endScale = data.value("endScale", Vector2{ 0.0f, 0.0f });
    state.scaleFade = data.value("scaleFade", 0u);
    state.scaleRandom = data.value("scaleRandom", 0u);
    state.minScale = data.value("minScale", Vector3{ 0.1f, 0.1f, 0.1f });
    state.maxScale = data.value("maxScale", Vector3{ 1.0f, 1.0f, 1.0f });

    // --- 回転関連 ---
    state.rotateMove = data.value("rotateMove", 0u);
    state.startRotateVelocity = data.value("startRotateVelocity", 0.0f);
    state.endRotateVelocity = data.value("endRotateVelocity", 0.0f);
    state.rotateVelocityRandom = data.value("rotateVelocityRandom", 0u);
    state.minRotateVelocity = data.value("minRotateVelocity", -1.0f);
    state.maxRotateVelocity = data.value("maxRotateVelocity", 1.0f);

    // --- アルファ・カラー関連 ---
    state.alphaFade = data.value("alphaFade", 0u);
    state.colorFade = data.value("colorFade", 0u);
    state.startColor = data.value("startColor", Vector3{ 1.0f, 1.0f, 1.0f });
    state.endColor = data.value("endColor", Vector3{ 0.0f, 0.0f, 1.0f });
    state.colorRandom = data.value("colorRandom", 0u);
    state.minColor = data.value("minColor", Vector3{ 0.0f, 0.0f, 0.0f });
    state.maxColor = data.value("maxColor", Vector3{ 1.0f, 1.0f, 1.0f });

    // --- 移動関連 ---
    state.isMove = data.value("isMove", 0u);
    state.startVelocity = data.value("startVelocity", Vector3{ 0.0f, 0.0f, 0.0f });
    state.endVelocity = data.value("endVelocity", Vector3{ 0.0f, 0.0f, 0.0f });
    state.velocityRandom = data.value("velocityRandom", 0u);
    state.minVelocity = data.value("minVelocity", Vector3{ -1.0f, -1.0f, -1.0f });
    state.maxVelocity = data.value("maxVelocity", Vector3{ 1.0f, 1.0f, 1.0f });

    // --- 寿命関連 ---
    state.lifeTime = data.value("lifeTime", 3.0f);
    state.lifeTimeRandom = data.value("lifeTimeRandom", 0u);
    state.minLifeTime = data.value("minLifeTime", 1.0f);
    state.maxLifeTime = data.value("maxLifeTime", 5.0f);

    return state;
}

// =============================================================
//                        Save
// =============================================================
void EmitterStateLoader::Save(const std::string& filePath, const EmitterSphere& state) {
    json data = {
        {"translate", state.translate},
        {"radius", state.radius},

        {"useEmitter", state.useEmitter},
        {"emit", state.emit},
        {"count", state.count},
        {"kMaxParticle", state.kMaxParticle},

        {"frequency", state.frequency},
        {"frequencyTime", state.frequencyTime},

        {"startScale", state.startScale},
        {"endScale", state.endScale},
        {"scaleFade", state.scaleFade},
        {"scaleRandom", state.scaleRandom},
        {"minScale", state.minScale},
        {"maxScale", state.maxScale},

        {"rotateMove", state.rotateMove},
        {"startRotateVelocity", state.startRotateVelocity},
        {"endRotateVelocity", state.endRotateVelocity},
        {"rotateVelocityRandom", state.rotateVelocityRandom},
        {"minRotateVelocity", state.minRotateVelocity},
        {"maxRotateVelocity", state.maxRotateVelocity},

        {"alphaFade", state.alphaFade},
        {"colorFade", state.colorFade},

        {"startColor", state.startColor},
        {"endColor", state.endColor},
        {"colorRandom", state.colorRandom},
        {"minColor", state.minColor},
        {"maxColor", state.maxColor},

        {"isMove", state.isMove},

        {"startVelocity", state.startVelocity},
        {"endVelocity", state.endVelocity},
        {"velocityRandom", state.velocityRandom},
        {"minVelocity", state.minVelocity},
        {"maxVelocity", state.maxVelocity},

        {"lifeTime", state.lifeTime},
        {"lifeTimeRandom", state.lifeTimeRandom},
        {"minLifeTime", state.minLifeTime},
        {"maxLifeTime", state.maxLifeTime}
    };

    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to save EmitterState file: " + filePath);
    }

    file << std::setw(4) << data;
}

// =============================================================
//               SaveToCurrentDir (自動出力版)
// =============================================================
void EmitterStateLoader::SaveToCurrentDir(const EmitterSphere& state, const std::string& ImGuiName) {
    try {
        fs::path currentDir = fs::current_path();
        fs::path outPath = currentDir / "Resources" / "Data" / "Particle" / (ImGuiName + ".json");

        printf("[DEBUG] currentDir: %s\n", currentDir.string().c_str());

        Save(outPath.string(), state);
        printf("[INFO] Emitter state saved to: %s\n", outPath.string().c_str());
    }
    catch (const std::exception& e) {
        printf("[ERROR] SaveToCurrentDir failed: %s\n", e.what());
    }
}