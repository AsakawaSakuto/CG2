#include "UpgradeManager.h"

void UpgradeManager::Initialize() {
	upgradeSelect_ = std::make_unique<Sprite>();
	upgradeSelect_->Initialize("UI/game/upgradeSelect.png");

	upgradeBG1_ = std::make_unique<Sprite>();
	upgradeBG1_->Initialize("UI/game/upgradeBG.png");
	upgradeBG2_ = std::make_unique<Sprite>();
	upgradeBG2_->Initialize("UI/game/upgradeBG.png");
	upgradeBG3_ = std::make_unique<Sprite>();
	upgradeBG3_->Initialize("UI/game/upgradeBG.png");

	upgradeIcon1_ = std::make_unique<Sprite>();
	upgradeIcon1_->Initialize("icon/none.png");
	upgradeIcon2_ = std::make_unique<Sprite>();
	upgradeIcon2_->Initialize("icon/none.png");
	upgradeIcon3_ = std::make_unique<Sprite>();
	upgradeIcon3_->Initialize("icon/none.png");
}

void UpgradeManager::Update() {
	upgradeSelect_->Update();
	upgradeBG1_->Update();
	upgradeBG2_->Update();
	upgradeBG3_->Update();
	upgradeIcon1_->Update();
	upgradeIcon2_->Update();
	upgradeIcon3_->Update();
}

void UpgradeManager::Draw() {
	upgradeSelect_->Draw();
	upgradeBG1_->Draw();
	upgradeBG2_->Draw();
	upgradeBG3_->Draw();
	upgradeIcon1_->Draw();
	upgradeIcon2_->Draw();
	upgradeIcon3_->Draw();
}

void UpgradeManager::DrawImGui() {
	upgradeSelect_->DrawImGui("UpgradeSelectUI");
	upgradeBG1_->DrawImGui("UpgradeBG1UI");
	upgradeBG2_->DrawImGui("UpgradeBG2UI");
	upgradeBG3_->DrawImGui("UpgradeBG3UI");
	upgradeIcon1_->DrawImGui("UpgradeIcon1UI");
	upgradeIcon2_->DrawImGui("UpgradeIcon2UI");
	upgradeIcon3_->DrawImGui("UpgradeIcon3UI");
}

void UpgradeManager::Upgrade() {
	// アップグレード処理
}