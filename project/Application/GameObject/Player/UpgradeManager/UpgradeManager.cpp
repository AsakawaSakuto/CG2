#include "UpgradeManager.h"

void UpgradeManager::Initialize() {
	upgradeSelectText_ = std::make_unique<Sprite>();
	upgradeSelectText_->Initialize("UI/game/upgradeSelect.png");
	upgradeSelectText_->LoadFromJson("upgradeSelectText");

	upgradeBG1_ = std::make_unique<Sprite>();
	upgradeBG1_->Initialize("UI/game/upgradeBG.png");
	upgradeBG1_->LoadFromJson("upgradeBG1");
	upgradeBG2_ = std::make_unique<Sprite>();
	upgradeBG2_->Initialize("UI/game/upgradeBG.png");
	upgradeBG2_->LoadFromJson("upgradeBG2");
	upgradeBG3_ = std::make_unique<Sprite>();
	upgradeBG3_->Initialize("UI/game/upgradeBG.png");
	upgradeBG3_->LoadFromJson("upgradeBG3");

	upgradeIcon1_ = std::make_unique<Sprite>();
	upgradeIcon1_->Initialize("icon/none.png");
	upgradeIcon1_->LoadFromJson("upgradeIcon1");
	upgradeIcon2_ = std::make_unique<Sprite>();
	upgradeIcon2_->Initialize("icon/none.png");
	upgradeIcon2_->LoadFromJson("upgradeIcon2");
	upgradeIcon3_ = std::make_unique<Sprite>();
	upgradeIcon3_->Initialize("icon/none.png");
	upgradeIcon3_->LoadFromJson("upgradeIcon3");

	bg_ = std::make_unique<Sprite>();
	bg_->Initialize("UI/game/selectBG.png");
	bg_->LoadFromJson("selectBG");

	isUpgrade_ = false;
}

void UpgradeManager::Update() {

	switch (upgradeSelect_) {
	case UpgradeSelect::Select1:

		if (MyInput::Trigger(Action::CELECT_DOWN)) {
			upgradeSelect_ = UpgradeSelect::Select2;
		}

		// この選択肢の強化をしてゲームを再開
		if (MyInput::Trigger(Action::CONFIRM)) {
			isUpgrade_ = false;
		}

		break;
	case UpgradeSelect::Select2:

		if (MyInput::Trigger(Action::CELECT_DOWN)) {
			upgradeSelect_ = UpgradeSelect::Select3;
		}

		if (MyInput::Trigger(Action::CELECT_UP)) {
			upgradeSelect_ = UpgradeSelect::Select1;
		}

		// この選択肢の強化をしてゲームを再開
		if (MyInput::Trigger(Action::CONFIRM)) {
			isUpgrade_ = false;
		}

		break;
	case UpgradeSelect::Select3:

		if (MyInput::Trigger(Action::CELECT_UP)) {
			upgradeSelect_ = UpgradeSelect::Select2;
		}

		// この選択肢の強化をしてゲームを再開
		if (MyInput::Trigger(Action::CONFIRM)) {
			isUpgrade_ = false;
		}

		break;
	}

	upgradeSelectText_->Update();
	upgradeBG1_->Update();
	upgradeBG2_->Update();
	upgradeBG3_->Update();
	upgradeIcon1_->Update();
	upgradeIcon2_->Update();
	upgradeIcon3_->Update();
	bg_->Update();
}

void UpgradeManager::Draw() {
	if (isUpgrade_) {
		bg_->Draw();
		upgradeSelectText_->Draw();
		upgradeBG1_->Draw();
		upgradeBG2_->Draw();
		upgradeBG3_->Draw();
		upgradeIcon1_->Draw();
		upgradeIcon2_->Draw();
		upgradeIcon3_->Draw();
	}
}

void UpgradeManager::DrawImGui() {
	//upgradeSelectText_->DrawImGui("UpgradeSelectUI");
	//upgradeBG1_->DrawImGui("UpgradeBG1UI");
	//upgradeBG2_->DrawImGui("UpgradeBG2UI");
	//upgradeBG3_->DrawImGui("UpgradeBG3UI");
	//upgradeIcon1_->DrawImGui("UpgradeIcon1UI");
	//upgradeIcon2_->DrawImGui("UpgradeIcon2UI");
	//upgradeIcon3_->DrawImGui("UpgradeIcon3UI");
	bg_->DrawImGui("UpgradeBGUI");
}