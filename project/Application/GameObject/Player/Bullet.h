#pragma once
#include "Application/GameObject/GameObject.h"
#include <memory>
#include <functional>

class Bullet : public GameObject {
public:

	void Initialize(DirectXCommon* dxCommon) override;
	void Update() override;
	void Draw(Camera useCamera) override;

	void Spawn(Vector3 position, float velocity);

	// デストラクション時のコールバック設定
	void SetOnDestroyCallback(std::function<void(Vector3)> callback) { onDestroyCallback_ = callback; }

	// 弾を削除する際に呼び出す
	void Destroy();

	// Getter
	//float GetSpeed() const { return speed_; }
	Transform& GetTransform() { return transform_; }

private:
	//float speed_ = 0.0f;
	float velocity_ = 0.0f;
	
	// デストラクション時のコールバック
	std::function<void(Vector3)> onDestroyCallback_;
};
