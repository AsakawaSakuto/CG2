#pragma once
#include "Application/GameObject/GameObject.h"
#include <array>

class Text3D {
public:
	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw(Camera& useCamera);

	void DrawImGui();
	~Text3D() {}

	bool IsEnd() { return timer_[12].IsFinished(); }
private:
	DirectXCommon* dxCommon_ = nullptr;

	std::array<std::unique_ptr<Model>, 13> model_;
	std::array<Transform, 13> transform_;
	std::array<GameTimer, 13> timer_;

	GameTimer muniStartTimer_;
	std::array<GameTimer, 13> muniTimer_;

	Vector4 okasiColor_ = { 1.000f, 0.482f, 0.953f, 1.000f };
	Vector4 atumeteColor_ = { 0.706f, 1.000f, 0.471f, 1.000f };
	Vector4 kumaColor_ = { 1.000f, 0.678f, 0.451f, 1.000f };
	Vector4 saidaColor_ = { 0.475f, 1.000f, 0.969f, 1.000f };
};