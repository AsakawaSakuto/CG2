#pragma once
#include "Application/GameObject/GameObject.h"
#include <array>

class TitleObject {
public:
	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw(Camera& useCamera);

	void DrawImGui();

	void PlayerStart() { playerStartTimer_.Start(1.0f); }
	~TitleObject() {}
private:
	DirectXCommon* dxCommon_ = nullptr;

	GameTimer playerStartTimer_;

	std::array<std::unique_ptr<Model>, 11> model_;
	std::array<Transform, 5> transform_;
};