#pragma once
#include "Application/EngineSystem.h"
#include "Application/GameObject/BaseGameObject.h"

class ExpItem : public BaseGameObject {
public:
	void Initialize(AppContext* ctx) override;
	void Update() override;
	void Draw(Camera camera) override;

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
	unique_ptr<Model> model_ = make_unique<Model>();
	GameTimer scaleTimer_;
	Vector3 targetPosition_ = {};
};