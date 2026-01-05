#include "PaidChest.h"

void PaidChest::Initialize(Vector3 pos) {
	if (isSpawn_) { return; }
	isSpawn_ = true;

	// モデルの読み込み
	model_ = std::make_unique<Model>();
	model_->Initialize("MapObject/chest/chest.obj");
	//model_->SetTexture("resources/image/white1x1.png");

	bottom_ = std::make_unique<Model>();
	bottom_->Initialize("MapObject/Chest/ChestIsActive.obj");
	bottom_->SetBillboard(true);
	bottom_->UseLight(false);

	// 位置設定
	transform_.translate = pos;
	transform_.rotate.y = MyRand::Float(0.0f, 10.0f);
	transform_.SetAllScale(0.85f);

	// 当たり判定設定
	aabbCollision_.center = pos;
	aabbCollision_.min = { -1.5f, 0.0f, -1.5f };
	aabbCollision_.max = {  1.5f, 1.0f,  1.5f };

	timer_.Start(1.0f, true);
}

void PaidChest::Update() {
	bottomTransform_.translate = transform_.translate + Vector3{ 0.0f, 2.0f, 0.0f };
	bottomTransform_.scale = MyEasing::Lerp_GAB(textMax, textMin, timer_.GetProgress());

	// タイマー更新
	timer_.Update();
}

void PaidChest::Draw(Camera camera) {
	if (isAlive_) {
		MyDebugLine::AddShape(aabbCollision_);
		model_->Draw(camera, transform_);

		if (chestIsActive_) {
			bottom_->Draw(camera, bottomTransform_);
		}
	}
}

void PaidChest::Open() {
	if (isAlive_) {
		isAlive_ = false;
		// 宝箱を開けたときの処理をここに追加
	}
}