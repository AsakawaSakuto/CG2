#pragma once
#include "EngineSystem.h"
#include "GameObject/BaseGameObject.h"

class ExpItem : public BaseGameObject {
public:
	void Initialize() override;
	void Update() override;
	void Draw(Camera camera) override;

	~ExpItem() override = default; // デストラクタを追加（unique_ptrの確実な解放を保証）

	void SetPosition(Vector3 pos) { transform_.translate = pos; }

	void SetTargetPosition(const Vector3& target) { targetPosition_ = target; }

	void StateChange() { state_ = State::MOVE;  }
private:

	enum class State {
		NONE,
		MOVE,
	};

	State state_ = State::NONE;

private:
	float speed_ = 10.0f;
	unique_ptr<SkiningModel> model_ = make_unique<SkiningModel>();
	GameTimer scaleTimer_;
	Vector3 targetPosition_ = {};
};