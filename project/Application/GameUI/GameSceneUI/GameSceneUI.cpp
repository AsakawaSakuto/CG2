#include "GameSceneUI.h"

void GameSceneUI::Initialize() {
	nowMoneyFont_ = std::make_unique<BitmapFont>();
	nowMoneyFont_->Initialize("nowMoney");

	expGauge_ = std::make_unique<Gauge>();
	expGauge_->Initialize("expGauge");
}

void GameSceneUI::Update() {
	nowMoneyFont_->SetNumber(nowMoneyValue_);
	expGauge_->Update(currentExpValue_, maxExpValue_);
}

void GameSceneUI::Draw() {
	nowMoneyFont_->Draw();
	expGauge_->Draw();
}

void GameSceneUI::DrawImGui() {
	//nowMoneyFont_->DrawImGui("NowMoneyFont");
	//expGauge_->DrawImGui("ExpGauge");
}