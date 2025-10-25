#pragma once
#include <array>
#include "Application/EngineSystem.h"

class Score
{
public:
	void Initialize(DirectXCommon* dxCommon,float score);

	void Update();

	void Draw(Camera camera);

	void DrawImGui();
private:
	DirectXCommon* dxCommon_ = nullptr;
	std::array<unique_ptr<Model>,8> textModel_;
	std::array<unique_ptr<Model>, 5> scoreModel_;
	std::array<Transform, 8> textTransform_;
	float score_ = 0.0f;
};