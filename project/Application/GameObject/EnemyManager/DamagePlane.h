#pragma once
#include "EngineSystem.h"

class DamagePlane {
public:

	/// <summary>
	/// 衝突した瞬間に位置とダメージを渡して生成
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="damage"></param>
	void Initialize(Vector3 pos, int damage);
	void Update();
	void Draw(Camera camera);

	bool IsAlive() const { return lifeTimer_.IsActive(); }

private:
	std::unique_ptr<Model> model_;
	Transform transform_;
	int damage_ = 0;
	GameTimer lifeTimer_;
	float lifeTime_ = 1.0f;  // 1秒間表示
};