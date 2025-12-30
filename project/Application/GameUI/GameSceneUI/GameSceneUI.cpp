#include "GameSceneUI.h"

void GameSceneUI::Initialize() {
	nowMoneyFont_ = std::make_unique<BitmapFont>();
	nowMoneyFont_->Initialize("nowMoney");

	expGauge_ = std::make_unique<Gauge>();
	expGauge_->Initialize("expGauge");

	hpGauge_ = std::make_unique<Gauge>();
	hpGauge_->Initialize("hpGauge");

	currentHpFont_ = std::make_unique<BitmapFont>();
	currentHpFont_->Initialize("nowHp");

	maxHpFont_ = std::make_unique<BitmapFont>();
	maxHpFont_->Initialize("maxHp");

	hpSrash_ = std::make_unique<Sprite>();
	hpSrash_->Initialize("bitMapFont/srash.png", { 137.0f, 65.0f }, { 0.2f, 0.2f });
}

void GameSceneUI::Update() {
	nowMoneyFont_->SetNumber(nowMoneyValue_);
	expGauge_->Update(currentExpValue_, maxExpValue_);
	hpGauge_->Update(currentHpValue_, maxHpValue_);
	currentHpFont_->SetNumber(static_cast<int>(currentHpValue_));
	maxHpFont_->SetNumber(static_cast<int>(maxHpValue_));
	hpSrash_->Update();
}

void GameSceneUI::Draw() {
	expGauge_->Draw();
	hpGauge_->Draw();
	nowMoneyFont_->Draw();
	currentHpFont_->Draw();
	maxHpFont_->Draw();
	hpSrash_->Draw();
}

void GameSceneUI::DrawImGui() {
	//nowMoneyFont_->DrawImGui("NowMoneyFont");
	//expGauge_->DrawImGui("ExpGauge");
	//hpGauge_->DrawImGui("HpGauge");
	//currentHpFont_->DrawImGui("NowHpFont");
	//maxHpFont_->DrawImGui("MaxHpFont");
	//hpSrash_->DrawImGui("HpSrash");
}