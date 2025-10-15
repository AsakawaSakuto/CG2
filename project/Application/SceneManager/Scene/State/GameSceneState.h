#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct GameSceneState {
	float maxNoInputTimer = 5.0f; // タイトルに戻るまでの猶予時間(秒)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameSceneState, maxNoInputTimer);
};