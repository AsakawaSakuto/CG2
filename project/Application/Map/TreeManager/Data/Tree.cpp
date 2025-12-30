#include "Tree.h"

void Tree::Initialize(Vector3 pos) {
	if (isSpawn_) { return; }
	isSpawn_ = true;
	// モデルの読み込み
	model_ = std::make_unique<Model>();
	model_->Initialize("MapObject/tree/tree.obj");
	
	treeType_ = TreeType(MyRand::Int(0, 2));

	// 位置設定
	transform_.translate = pos;
	transform_.rotate.y = MyRand::Float(0.0f, 10.0f);
	aabbCollision_.center = pos;
	
	switch (treeType_) {
	case TreeType::Min:
		transform_.SetAllScale(2.0f);
		aabbCollision_.min = { -0.3f, 0.0f, -0.35f };
		aabbCollision_.max = {  0.3f, 10.0f,  0.35f };
		break;
	case TreeType::Middle:
		transform_.SetAllScale(3.0f);
		aabbCollision_.min = { -0.45f, 0.0f, -0.45f };
		aabbCollision_.max = {  0.45f, 10.0f,  0.45f };
		break;
	case TreeType::Max:
		transform_.SetAllScale(4.0f);
		aabbCollision_.min = { -0.6f, 0.0f, -0.6f };
		aabbCollision_.max = {  0.6f, 10.0f,  0.6f };
		break;
	}
}

void Tree::Draw(Camera camera) {
	if (isSpawn_) {
		MyDebugLine::AddShape(aabbCollision_);
		model_->Draw(camera, transform_);
	}
}

void Tree::SetTransparent(bool enable, float alpha) {
	if (model_) {
		model_->SetTransparent(enable);
		if (enable) {
			model_->SetColor4({1.0f, 1.0f, 1.0f, alpha});
		} else {
			model_->SetColor4({1.0f, 1.0f, 1.0f, 1.0f});
		}
	}
}