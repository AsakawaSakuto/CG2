#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Sprite.h"
#include"Camera.h"
#include"MatrixFunction.h"

using std::unique_ptr;
using std::make_unique;

class SkyBox
{
public:
	
	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();
private:
	DirectXCommon* dxCommon_ = nullptr;
	unique_ptr<Object3d> left_ = make_unique<Object3d>();
	unique_ptr<Object3d> right_ = make_unique<Object3d>();
	unique_ptr<Object3d> top_ = make_unique<Object3d>();
	unique_ptr<Object3d> bottom_ = make_unique<Object3d>();

	float deltaTime_ = 1.0f / 60.0f;
	float uvSpeed_ = 0.5f;

	Vector2 uvTranslateLeft_ = {};
	Vector2 uvTranslateRight_ = {};
	Vector2 uvTranslateTop_ = {};
	Vector2 uvTranslateBottom_ = {};
};