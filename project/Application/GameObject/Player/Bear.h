#pragma once
#include "Application/GameObject/GameObject.h"

class Bear : public GameObject{
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Bear() {}

	// Setter
	void SetTranslate(Vector3 translate) { transform_.translate = translate; }
	void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }
	void SetOffsetX(float offsetX) { offsetX_ = offsetX; }

	void ImGuiUpdate();

private:
	void BearAnimation();

private:
	std::unique_ptr<Model> modelBearHead_ = std::make_unique<Model>();
	std::unique_ptr<Model> modelBearBody_ = std::make_unique<Model>();
	std::unique_ptr<Model> modelBearArmR_ = std::make_unique<Model>();
	std::unique_ptr<Model> modelBearArmL_ = std::make_unique<Model>();
	std::unique_ptr<Model> modelBearLegR_ = std::make_unique<Model>();
	std::unique_ptr<Model> modelBearLegL_ = std::make_unique<Model>();

	// ローカルTransform
	Transform localTransform_[6];

	// オフセット　自機の反転時にクマのモデルがずれてしまうので対策
	float offsetX_;

	// クマのモデル回転スピード
	float rotateSpeed_ = 0.01f;
};
