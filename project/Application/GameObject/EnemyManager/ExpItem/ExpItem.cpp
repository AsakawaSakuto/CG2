#include "ExpItem.h"

void ExpItem::Initialize() {

	model_->Initialize("Animation/ExpItem/ExpItem.gltf");
	model_->SetTexture("resources/image/white16x16.png");
	model_->SetColor3({ 166.0f / 255.0f,245.0f / 255.0f,233.0f / 255.0f });

	transform_.scale = { 0.5f,0.5f,0.5f };

	scaleTimer_.Start(2.0f, false);
}

void ExpItem::Update() {

	transform_.scale = Easing::Lerp(
		Vector3({0.0f,0.0f,0.0f}), Vector3({ 0.5f,0.5f,0.5f }),
		scaleTimer_.GetProgress(), Easing::Type::EaseOutBack);

	if (state_ == State::MOVE) {
		// .xz平面でターゲット位置への方向ベクトルを計算（yは無視）
		Vector3 direction = {
			targetPosition_.x - transform_.translate.x,
			targetPosition_.y - transform_.translate.y,
			targetPosition_.z - transform_.translate.z
		};

		// .xz平面での距離を計算
		float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

		// ターゲットに到達していない場合のみ移動と回転
		if (distance > 0.01f) { // 微小な閾値で到達判定
			// .xz平面で正規化（yは0のまま）
			direction.x /= distance; 
			direction.y /= distance;
			direction.z /= distance;

			// 移動速度を適用
			Vector3 velocity = {
				direction.x * speed_ * deltaTime_,
				direction.y * speed_* deltaTime_,
				direction.z * speed_ * deltaTime_
			};

			// 位置を更新（yはそのまま）
			transform_.translate.x += velocity.x;
			transform_.translate.y += velocity.y;
			transform_.translate.z += velocity.z;
		}
	}

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = 0.5f * transform_.scale.x;

	scaleTimer_.Update();

	model_->Update();
}

void ExpItem::Draw(Camera camera) {
	if (scaleTimer_.GetProgress() >= 0.1f) {
		model_->Draw(camera, transform_);
	}
}