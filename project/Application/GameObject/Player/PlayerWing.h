#pragma once
#include "Application/GameObject/GameObject.h"

class PlayerWing : public GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~PlayerWing() {}

	// Getter
	Vector3 GetTranslate() { return transform_.translate; }

	// Setter
	void SetPosition(Vector3 position) { transform_.translate = position; }
	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }

	void WingImGui();

private:
	// 当たり判定更新(AABB)
	void CollisionUpdate();

	// 腕のモデル
	std::unique_ptr<Model> modelArmR01_;
	std::unique_ptr<Model> modelArmR02_;
	std::unique_ptr<Model> modelArmL01_;
	std::unique_ptr<Model> modelArmL02_;

	float testX_{};
	float testY_{};
};
