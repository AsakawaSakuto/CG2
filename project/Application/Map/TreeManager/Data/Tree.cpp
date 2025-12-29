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
		transform_.SetAllScale(2.0f);
		aabbCollision_.min = { -0.3f, 0.0f, -0.35f };
		aabbCollision_.max = {  0.3f, 4.0f,  0.35f };
		break;
	case TreeType::Middle:
		transform_.SetAllScale(3.0f);
		aabbCollision_.min = { -0.45f, 0.0f, -0.45f };
		aabbCollision_.max = {  0.45f, 6.0f,  0.45f };
		break;
	case TreeType::Max:
		transform_.SetAllScale(4.0f);
		aabbCollision_.min = { -0.6f, 0.0f, -0.6f };
		aabbCollision_.max = {  0.6f, 8.0f,  0.6f };
		break;
	}
}

void Tree::Draw(Camera camera) {
	if (isSpawn_) {
		MyDebugLine::AddShape(aabbCollision_);
		model_->Draw(camera, transform_);
	}
}