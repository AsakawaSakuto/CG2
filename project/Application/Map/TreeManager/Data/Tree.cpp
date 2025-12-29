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
	aabbCollision_.center = pos;
	
	switch (treeType_) {
	case TreeType::Min:
		transform_.SetAllScale(1.0f);
		aabbCollision_.min = { -0.15f, 0.0f, -0.15f };
		aabbCollision_.max = {  0.15f, 2.0f,  0.15f };
		break;
	case TreeType::Middle:
		transform_.SetAllScale(1.5f);
		aabbCollision_.min = { -0.15f, 0.0f, -0.15f };
		aabbCollision_.max = { 0.15f,  3.0f,  0.15f };
		break;
	case TreeType::Max:
		transform_.SetAllScale(2.0f);
		aabbCollision_.min = { -0.15f, 0.0f, -0.15f };
		aabbCollision_.max = { 0.15f,  4.0f,  0.15f };
		break;
	}
}

void Tree::Draw(Camera camera) {
	if (isSpawn_) {
		MyDebugLine::AddShape(aabbCollision_);
		model_->Draw(camera, transform_);
	}
}