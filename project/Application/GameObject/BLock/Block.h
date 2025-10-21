#pragma once
#include "Application/GameObject/GameObject.h"

class Block : public GameObject{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Block() {}

	// 生成時の位置設定
	void Spawn(Vector3 position);

	// Getter
	Transform& GetTransform() { return transform_; }
	float GetScoreAmount() const { return scoreAmount_; }

	// Setter
	void SetPosition(Vector3 position) { transform_.translate = position; }

private:
	// スコア加算量
	float scoreAmount_ = 100.0f;
};
