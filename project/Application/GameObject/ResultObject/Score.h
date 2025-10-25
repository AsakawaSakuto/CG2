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

	void InitTextModel();
	void InitScoreModel();
	void InitRankModel();
	void InitPlayerModel();

private:
	DirectXCommon* dxCommon_ = nullptr;
	std::array<unique_ptr<Model>,8> textModel_;
	std::array<unique_ptr<Model>, 5> scoreModel_;
	std::array<unique_ptr<Model>, 2> rankModel_;
	unique_ptr<Model> playerModel_;
	unique_ptr<Model> machineModel_;
	std::array<Transform, 8> textTransform_;
	std::array<Transform, 5> scoreTransform_;
	std::array<Transform, 2> rankTransform_;
	Transform playerTransform_;
	Transform machineTransform_;
	float score_ = 0.0f;
};