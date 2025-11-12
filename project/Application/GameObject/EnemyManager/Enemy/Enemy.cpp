#include "Enemy.h"

void Enemy::Initialize(AppContext* ctx) {
	ctx_ = ctx;
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.translate = { 0.f,0.f,0.f };

	model_->Initialize(&ctx_->dxCommon, "enemy/enemy.obj");
	model_->SetUpdateFrustumCulling(true);
	//model_->SetDrawFrustumCulling(true);
}

void Enemy::Update() {
	
	Move();

	model_->SetTransform(transform_);
	model_->Update();
}

void Enemy::Draw(Camera camera) {
	model_->Draw(camera);
}

void Enemy::DrawImGui() {

}

void Enemy::Move() {
	// zx平面でターゲット位置への方向ベクトルを計算（yは無視）
	Vector3 direction = {
		targetPosition_.x - transform_.translate.x,
		0.0f, // y成分は0にして水平移動のみ
		targetPosition_.z - transform_.translate.z
	};

	// zx平面での距離を計算
	float distanceXZ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

	// ターゲットに到達していない場合のみ移動と回転
	if (distanceXZ > 0.01f) { // 微小な閾値で到達判定
		// zx平面で正規化（yは0のまま）
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