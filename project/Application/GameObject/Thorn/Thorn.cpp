#include "Thorn.h"
#include <numbers>

void Thorn::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "Enemy/Enemy/Enemy.obj");

	transform_.scale = {1.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, std::numbers::pi_v<float> / 2.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};

	// 当たり判定(AABB)
	Vector3 t = transform_.translate;
	collisionAABB_.max = {t.x + 1.0f, t.y + 1.0f, t.z + 1.0f};
	collisionAABB_.min = {t.x - 1.0f, t.y - 1.0f, t.z - 1.0f};

	collisionSphere_.center = transform_.translate;
	collisionSphere_.radius = 0.25f;

	particle_->Initialize(dxCommon_);
}

void Thorn::Update() {
	if (!isAlive_)
		return;

	// トゲのサイズを更新
	//UpgradeThorn();

	if (scaleTween_.active) {
		scaleTween_.elapsed += 1.7f * deltaTime_;
		float t = std::min(scaleTween_.elapsed / scaleTween_.duration, 1.0f);

		transform_.scale.x = Easing::Lerp(scaleTween_.startScale.x, scaleTween_.targetScale.x, t, Easing::Type::EaseInElastic);
		transform_.scale.y = Easing::Lerp(scaleTween_.startScale.y, scaleTween_.targetScale.y, t, Easing::Type::EaseInElastic);
		transform_.scale.z = Easing::Lerp(scaleTween_.startScale.z, scaleTween_.targetScale.z, t, Easing::Type::EaseInElastic);

		if (t >= 1.0f) {
			scaleTween_.active = false;
		}
	}


	model_->SetTransform(transform_);
	model_->Update();
}

void Thorn::SetThornType(ThornType type) {
	if (type_ == type)
		return;

	type_ = type;

	// タイプ変更が起きたらイージングを開始する
	UpgradeThorn();
}

void Thorn::Draw(Camera useCamera) { model_->Draw(useCamera); }

void Thorn::Spawn(Vector3 position) {
	if (isAlive_)
		return;
	else {
		isAlive_ = true;
		transform_.translate = position;
		collisionSphere_.center = transform_.translate;

		Vector3 t = transform_.translate;
		collisionAABB_.max = {t.x + 1.0f, t.y + 1.0f, t.z + 1.0f};
		collisionAABB_.min = {t.x - 1.0f, t.y - 1.0f, t.z - 1.0f};
	}
}

void Thorn::TickCooldown() {
	if (upgradeCooldownFramesBullet_ > 0) {
		--upgradeCooldownFramesBullet_;
	}
}

void Thorn::UpgradeThorn() {
	Vector3 newScale;

	switch (type_) {
	case ThornType::MIN:
		newScale = {1.0f, 1.0f, 1.0f};
		collisionSphere_.radius = 0.25f;
		break;
	case ThornType::MIDDLE:
		newScale = {1.25f, 1.25f, 1.25f};
		collisionSphere_.radius = 0.35f;
		break;
	case ThornType::MAX:
		newScale = {1.5f, 1.5f, 1.5f};
		collisionSphere_.radius = 0.5f;
		break;
	}

	scaleTween_.startScale = transform_.scale;
	scaleTween_.targetScale = newScale;
	scaleTween_.duration = 0.6f;
	scaleTween_.elapsed = 0.0f;
	scaleTween_.active = true;
}
