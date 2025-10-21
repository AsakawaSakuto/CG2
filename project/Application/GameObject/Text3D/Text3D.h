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
};