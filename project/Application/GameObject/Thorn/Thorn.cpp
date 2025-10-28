#include "Thorn.h"
#include <numbers>

void Thorn::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	model_->Initialize(dxCommon_, "Enemy/Enemy/Enemy.obj");
	model_->SetColor({1.0f, 0.2f, 0.6f, 1.0f});

	transform_.scale = {1.0f, 1.0f, 1.0f};
	transform_.rotate = {0.0f, std::numbers::pi_v<float> / 2.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, 0.0f};

	// 当たり判定(AABB)
	Vector3 position = transform_.translate;
	collisionAABB_.max = {position.x + 1.0f, position.y + 1.0f, position.z + 1.0f};
	collisionAABB_.min = {position.x - 1.0f, position.y - 1.0f, position.z - 1.0f};

	collisionSphere_.center = transform_.translate;
	collisionSphere_.radius = 0.25f;

	particle_->Initialize(dxCommon_);

	// 回転フラグ
	isRotate_ = false;

	// 揺れフラグ
	isShaking_ = false;
}

void Thorn::Update() {
	if (!isAlive_)
		return;

	if (scaleTween_.active) {
		scaleTween_.elapsed += 1.5f * deltaTime_;
		float t = std::min(scaleTween_.elapsed / scaleTween_.duration, 1.0f);

		transform_.scale.x = Easing::Lerp(scaleTween_.startScale.x, scaleTween_.targetScale.x, t, Easing::Type::EaseInElastic);
		transform_.scale.y = Easing::Lerp(scaleTween_.startScale.y, scaleTween_.targetScale.y, t, Easing::Type::EaseInElastic);
		transform_.scale.z = Easing::Lerp(scaleTween_.startScale.z, scaleTween_.targetScale.z, t, Easing::Type::EaseInElastic);

		Vector3 position = transform_.translate;
		collisionAABB_.max = {position.x + transform_.scale.x / 1.3f, position.y + transform_.scale.y / 1.3f, position.z + transform_.scale.z / 1.3f };
		collisionAABB_.min = {position.x - transform_.scale.x / 1.7f, position.y - transform_.scale.y / 1.7f, position.z - transform_.scale.z / 1.7f };

		if (t >= 1.0f) {
			scaleTween_.active = false;
		}
	}

	// 座標揺れの更新
	if (isShaking_) {
		shakeElapsed_ += deltaTime_;
		float t = std::min(shakeElapsed_ / shakeDuration_, 1.0f);

		// 揺れ
		float offsetX = std::sin(t * 32.0f) * shakeAmplitude_ * (1.0f - t);

		transform_.translate.x = basePosition_.x + offsetX;

		if (t >= 1.0f) {
			isShaking_ = false;
			transform_.translate.x = basePosition_.x;
		}
	}


	// 回転の更新
	UpdateRotate();

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

		// ベースのポジションを記録
		basePosition_ = t;
	}
}

void Thorn::TickCooldown() {
	if (upgradeCooldownFramesBullet_ > 0) {
		--upgradeCooldownFramesBullet_;
	}
}

void Thorn::UpdateRotate() {
	if (isRotate_) {
		// 回転
		transform_.rotate.z += rotateSpeed_ * deltaTime_;

		// 回転速度を減衰させる
		const float kAttenuationRate = 0.9f;
		rotateSpeed_ *= kAttenuationRate;

		// 一定以下になったら停止
		if (rotateSpeed_ < 0.01f) {
			rotateSpeed_ = 0.0f;
			isRotate_ = false;
			transform_.rotate.z = 0.0f;
		}
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
		newScale = {2.5f, 2.5f, 2.5f};
		collisionSphere_.radius = 0.35f;
		break;
	case ThornType::MAX:
		newScale = {3.5f, 3.5f, 3.5f};
		collisionSphere_.radius = 0.5f;
		break;
	}

	scaleTween_.startScale = transform_.scale;
	scaleTween_.targetScale = newScale;
	scaleTween_.duration = 0.4f;
	scaleTween_.elapsed = 0.0f;
	scaleTween_.active = true;
}
