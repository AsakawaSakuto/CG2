#pragma once
#include "Application/GameObject/GameObject.h"

enum class ThornType
{
	MIN = 0,
	MIDDLE = 1,
	MAX = 2,
};

class Thorn : public GameObject {
public:
	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;
	~Thorn() {}

	bool GetIsAlive() const { return isAlive_; }

	Transform& GetTransform() { return transform_; }

	// 生成時の位置設定
	void Spawn(Vector3 position);

	// Getter
	ThornType GetThornType() const { return type_; }

	// Setter
	void SetPosition(Vector3 position) { transform_.translate = position; }
	void SetThornType(ThornType type) { type_ = type; }

private:
	void UpgradeThorn();

private:
	// トゲのタイプ
	ThornType type_ = ThornType::MIN;
};
