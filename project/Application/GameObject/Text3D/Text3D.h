#pragma once
#include "Application/GameObject/GameObject.h"

class Text3D : public GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override final;
	void Update() override final;
	void Draw(Camera useCamera) override final;

	~Text3D() {}

	// テキストの移動アニメーション
	void MoveTextAnimation(const float startPosX, const float endPosX, const float startRotY, const float endRotY);

private:
	// アニメーションの開始位置、終了位置
	//const float START_POS_X = 0.0f;
	//const float END_POS_Y = 0.0f;

	//// アニメーションの開始時回転、終了時回転
	//const float START_ROT_X = 0.0f;
	//const float END_ROT_Y = 0.0f;

	float duration_ = 5.0f;
	float timer_ = 0.0f;
};