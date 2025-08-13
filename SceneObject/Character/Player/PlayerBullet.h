#pragma once
#include"DirectXCommon.h"
#include"Object3d.h"
#include"Camera.h"

using std::unique_ptr;
using std::make_unique;

class PlayerBullet
{
public:

	void Initialize(DirectXCommon* dxCommon);

	void Update(Camera* camera);

	void Draw();

	void DrawImGui();

	void Spawn(Vector3 translate, Vector3 velocity);

	bool GetIsAlive() { return isAlive_; }

	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }

private:
	DirectXCommon* dxCommon_ = nullptr;

	unique_ptr<Object3d> model_ = make_unique<Object3d>();

	float deltaTime_ = 1.0f / 60.0f;

	float lifeTimer_ = 0.0f;

	float lifeTime_ = 5.0f;

	bool isAlive_ = false;

	Vector3 velocity_ = {};
};