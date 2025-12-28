#include "PaidChest.h"

void PaidChest::Initialize(Vector3 pos) {
	if (isSpawn_) { return; }
	isSpawn_ = true;

	// モデルの読み込み
	model_ = std::make_unique<Model>();
	model_->Initialize("MapObject/chest/chest.obj");
	//model_->SetTexture("resources/image/white1x1.png");

	// 位置設定
	transform_.translate = pos;
	transform_.rotate.y = MyRand::Float(0.0f, 10.0f);
	transform_.SetAllScale(0.85f);

	// 当たり判定設定
	aabbCollision_.center = pos;
	aabbCollision_.min = { -0.5f, 0.0f, -0.5f };
	aabbCollision_.max = { 0.5f, 1.0f, 0.5f };
}

void PaidChest::Draw(Camera camera) {
	if (isAlive_) {
		MyDebugLine::AddShape(aabbCollision_);
		model_->Draw(camera, transform_);
	}
}

void PaidChest::Open() {
	if (isAlive_) {
		isAlive_ = false;
		// 宝箱を開けたときの処理をここに追加
	}
}