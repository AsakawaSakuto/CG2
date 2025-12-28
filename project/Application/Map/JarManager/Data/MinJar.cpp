#include "MinJar.h"

void MinJar::Initialize(Vector3 pos) {

	if (isSpawn_) { return; }
	isSpawn_ = true;

	// モデルの読み込み
	model_ = std::make_unique<Model>();
	model_->Initialize("jar/minjar.obj");

	// 位置設定
	transform_.translate = pos;
	
	// 当たり判定設定
	aabbCollision_.center = pos;
	aabbCollision_.min = { -0.25f, 0.0f, -0.25f };
	aabbCollision_.max = { 0.25f, 0.5f, 0.25f };

	// ドロップアイテム設定
	dropExpMin_   = 5;
	dropExpMax_   = 10;

	dropMoneyMin_ = 10;
	dropMoneyMax_ = 20;

	if (MyRand::Int(0, 1) == 0) {
		jarType_ = JarType::Exp;
	} else {
		jarType_ = JarType::Money;
	}
}

void MinJar::Draw(Camera camera) {
	if (isAlive_) {
		MyDebugLine::AddShape(aabbCollision_);
		model_->Draw(camera, transform_);
	}
}

int MinJar::Break() {
	if (isAlive_) {
		isAlive_ = false;

		switch (jarType_)
		{
		case JarType::Exp:
			{
				int exp = MyRand::Int(dropExpMin_, dropExpMax_);
				return exp;
			}
			break;
		case JarType::Money:
			{
				int money = MyRand::Int(dropMoneyMin_, dropMoneyMax_);
				return money;
			}
			break;
		}
	}
	
	return 0;
}