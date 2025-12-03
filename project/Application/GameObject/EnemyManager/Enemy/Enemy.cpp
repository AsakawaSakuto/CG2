#include "Enemy.h"

void Enemy::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.translate = { 0.0f,0.0f,0.0f };

	model_->Initialize(&ctx_->dxCommon, "Animation/human/lowWalk.gltf");
	model_->SetUpdateFrustumCulling(false);
	//model_->SetDrawFrustumCulling(false);

	moveSpeed_ = 2.0f;
	collicionRadius_ = 0.5f;

	scaleTimer_.Start(0.5f, false);

	debugLine_->Initialize(&ctx_->dxCommon);
}

void Enemy::Update() {
	
	Move();

	model_->Update();

	transform_.scale = Easing::Lerp(Vector3({0.0f,0.0f,0.0f}), Vector3({ 1.0f,1.0f,1.0f }), 
		scaleTimer_.GetProgress(), Easing::Type::Linear);

	sphereCollision_.center = transform_.translate;
	sphereCollision_.radius = collicionRadius_;

	scaleTimer_.Update();

	debugLine_->AddSphere(sphereCollision_);
}

void Enemy::Draw(Camera camera) {
	if (scaleTimer_.GetProgress() >= 0.1f) {
		debugLine_->Draw(camera);

		model_->Draw(camera, transform_);
	}
}

void Enemy::DrawImGui() {

}

void Enemy::Move() {
	// .xz平面でターゲット位置への方向ベクトルを計算（yは無視）
	Vector3 direction = {
		targetPosition_.x - transform_.translate.x,
		0.0f, // y成分は0にして水平移動のみ
		targetPosition_.z - transform_.translate.z
	};

	// .xz平面での距離を計算
	float distanceXZ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

	// ターゲットに到達していない場合のみ移動と回転
	if (distanceXZ > 0.01f) { // 微小な閾値で到達判定
		// .xz平面で正規化（yは0のまま）
		direction.x /= distanceXZ;
		direction.z /= distanceXZ;

		// ターゲット方向を向くように回転を計算
		// atan2(x,z)でY軸周りの回転角度を求める
		transform_.rotate.y = std::atan2(direction.x, direction.z);

		// 移動速度を適用
		Vector3 velocity = {
			direction.x * moveSpeed_ * deltaTime_,
			0.0f, // y方向には移動しない
			direction.z * moveSpeed_ * deltaTime_
		};

		// 位置を更新（yはそのまま）
		transform_.translate.x += velocity.x;
		transform_.translate.z += velocity.z;
	}
}

void Enemy::PushAway(const Vector3& otherPosition, float otherRadius) {
	// 自分と他のEnemyとの方向ベクトルを計算（xz平面のみ）
	Vector3 direction = {
		transform_.translate.x - otherPosition.x,
		0.0f,
		transform_.translate.z - otherPosition.z
	};

	// .xz平面での距離を計算
	float distance = std::sqrt(direction.x * direction.x + direction.z * direction.z);

	// 重なり量を計算
	float radiusSum = collicionRadius_ + otherRadius;
	float overlap = radiusSum - distance;

	// 重なっている場合のみ押し出す
	if (overlap > 0.0f && distance > 0.01f) {
		// 正規化
		direction.x /= distance;
		direction.z /= distance;

		// 押し出す距離の半分ずつ移動（お互いに押し合う）
		float pushDistance = overlap * 0.5f;
		transform_.translate.x += direction.x * pushDistance;
		transform_.translate.z += direction.z * pushDistance;
	}
}