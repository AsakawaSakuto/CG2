#include "MaxJar.h"

void MaxJar::Initialize(Vector3 pos) {

	if (isSpawn_) { return; }
	isSpawn_ = true;

	// モデルの読み込み
	model_ = std::make_unique<Model>();
	model_->Initialize("MapObject/jar/maxjar.obj");
	model_->SetTexture("resources/image/white1x1.png");

	// 位置設定
	transform_.translate = pos;
	transform_.scale = { 1.1f, 1.1f, 1.1f };

	// 当たり判定設定
	aabbCollision_.center = pos;
	aabbCollision_.min = { -1.0f, 0.0f, -1.0f };
	aabbCollision_.max = {  1.0f, 1.0f,  1.0f };

	// ドロップアイテム設定
	dropExpMin_ = 5;
	dropExpMax_ = 10;

	dropMoneyMin_ = 10;
	dropMoneyMax_ = 20;

	if (MyRand::Int(0, 1) == 0) {
		jarType_ = JarType::Exp;
		model_->SetColor3({ 0.0f, 0.0f, 1.0f });
	} else {
		jarType_ = JarType::Money;
		model_->SetColor3({ 1.0f, 1.0f, 0.0f });
	}
}

void MaxJar::Draw(Camera camera) {
	if (isAlive_) {
		MyDebugLine::AddShape(aabbCollision_);
		model_->Draw(camera, transform_);
	}
}

int MaxJar::Break() {
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