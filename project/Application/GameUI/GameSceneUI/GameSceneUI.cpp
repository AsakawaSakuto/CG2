#include "GameSceneUI.h"

void GameSceneUI::Initialize() {
	moneyFont_ = std::make_unique<BitmapFont>();
	moneyFont_->Initialize("nowMoney");

	money_ = std::make_unique<Sprite>();
	money_->Initialize("UI/Game/money.png");
	money_->LoadFromJson("money");
	money_->SetAnchorPoint(AnchorPoint::Center);

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
	hpSrash_->SetAnchorPoint(AnchorPoint::Center);

	lv_ = std::make_unique<Sprite>();
	lv_->Initialize("UI/Game/lv.png");
	lv_->LoadFromJson("lv");
	lv_->SetAnchorPoint(AnchorPoint::Center);

	lvFont_ = std::make_unique<BitmapFont>();
	lvFont_->Initialize("lvFont");

	enemy_ = std::make_unique<Sprite>();
	enemy_->Initialize("UI/Game/enemy.png");
	enemy_->LoadFromJson("enemy");
	enemy_->SetAnchorPoint(AnchorPoint::Center);

	enemyFont_ = std::make_unique<BitmapFont>();
	enemyFont_->Initialize("enemy");

	weaponIcon1_ = std::make_unique<Sprite>();
	weaponIcon1_->Initialize("icon/none.png");
	weaponIcon2_ = std::make_unique<Sprite>();
	weaponIcon2_->Initialize("icon/none.png");
	weaponIcon3_ = std::make_unique<Sprite>();
	weaponIcon3_->Initialize("icon/none.png");
	weaponIcon4_ = std::make_unique<Sprite>();
	weaponIcon4_->Initialize("icon/none.png");

	weaponIcon1_->LoadFromJson("wep1");
	weaponIcon2_->LoadFromJson("wep2");
	weaponIcon3_->LoadFromJson("wep3");
	weaponIcon4_->LoadFromJson("wep4");

	playTimeFont_ = std::make_unique<BitmapFont>();
	playTimeFont_->Initialize("playTime");

	chestIcon_ = std::make_unique<Sprite>();
	chestIcon_->Initialize("UI/Game/chest.png");
	chestIcon_->LoadFromJson("chestIcon");

	chestCostFont_ = std::make_unique<BitmapFont>();
	chestCostFont_->Initialize("chestCost");
	chestCostFont_->LoadJson("chestFont");

	weaponLv1_ = std::make_unique<Sprite>();
	weaponLv2_ = std::make_unique<Sprite>();
	weaponLv3_ = std::make_unique<Sprite>();
	weaponLv4_ = std::make_unique<Sprite>();
	weaponLv1_->Initialize("UI/Game/Lv.png");
	weaponLv2_->Initialize("UI/Game/Lv.png");
	weaponLv3_->Initialize("UI/Game/Lv.png");
	weaponLv4_->Initialize("UI/Game/Lv.png");
	weaponLv1_->LoadFromJson("wLv1");
	weaponLv2_->LoadFromJson("wLv2");
	weaponLv3_->LoadFromJson("wLv3");
	weaponLv4_->LoadFromJson("wLv4");
	weaponLvFont1_ = std::make_unique<BitmapFont>();
	weaponLvFont2_ = std::make_unique<BitmapFont>();
	weaponLvFont3_ = std::make_unique<BitmapFont>();
	weaponLvFont4_ = std::make_unique<BitmapFont>();
	weaponLvFont1_->Initialize();
	weaponLvFont2_->Initialize();
	weaponLvFont3_->Initialize();
	weaponLvFont4_->Initialize();
	weaponLvFont1_->LoadJson("wLvFont1");
	weaponLvFont2_->LoadJson("wLvFont2");
	weaponLvFont3_->LoadJson("wLvFont3");
	weaponLvFont4_->LoadJson("wLvFont4");

	pauseBg_ = std::make_unique<Sprite>();
	pauseBg_->Initialize("UI/game/pauseBg.png");
	pauseBg_->LoadFromJson("pauseBg");
	back_ = std::make_unique<Sprite>();
	back_->Initialize("UI/game/back.png");
	back_->LoadFromJson("back");
	restart_ = std::make_unique<Sprite>();
	restart_->Initialize("UI/game/restart.png");
	restart_->LoadFromJson("restart");
	goTitle_ = std::make_unique<Sprite>();
	goTitle_->Initialize("UI/game/quit.png");
	goTitle_->LoadFromJson("goTitle");
}

void GameSceneUI::Update() {
	moneyFont_->SetNumber(moneyValue_);
	money_->Update();
	expGauge_->Update(currentExpValue_, maxExpValue_);
	hpGauge_->Update(currentHpValue_, maxHpValue_);
	currentHpFont_->SetNumber(static_cast<int>(currentHpValue_));
	maxHpFont_->SetNumber(static_cast<int>(maxHpValue_));
	hpSrash_->Update();
	lvFont_->SetNumber(nowLv_);
	lv_->Update();
	enemyFont_->SetNumber(killEnemyValue_);
	enemy_->Update();

	playTimeFont_->SetTime(playTime_);

	chestIcon_->Update();

	weaponIcon1_->Update();
	weaponIcon2_->Update();
	weaponIcon3_->Update();
	weaponIcon4_->Update();

	weaponLv1_->Update();
	weaponLv2_->Update();
	weaponLv3_->Update();
	weaponLv4_->Update();

	switch (pauseType_) {
	case PauseType::Back:
		back_->SetScale(pauseMax_);
		restart_->SetScale(pauseMin_);
		goTitle_->SetScale(pauseMin_);
		break;
	case PauseType::ReStart:
		back_->SetScale(pauseMin_);
		restart_->SetScale(pauseMax_);
		goTitle_->SetScale(pauseMin_);
		break;
	case PauseType::GoTitle:
		back_->SetScale(pauseMin_);
		restart_->SetScale(pauseMin_);
		goTitle_->SetScale(pauseMax_);
		break;
	}

	pauseBg_->Update();
	back_->Update();
	restart_->Update();
	goTitle_->Update();
}

void GameSceneUI::Draw() {
	expGauge_->Draw();

	hpGauge_->Draw();
	currentHpFont_->Draw();
	maxHpFont_->Draw();
	hpSrash_->Draw();

	moneyFont_->Draw();
	money_->Draw();

	lvFont_->Draw();
	lv_->Draw();

	enemyFont_->Draw();
	enemy_->Draw();

	weaponIcon1_->Draw();
	weaponIcon2_->Draw();
	weaponIcon3_->Draw();
	weaponIcon4_->Draw();

	chestIcon_->Draw();
	chestCostFont_->Draw();

	playTimeFont_->Draw();

	// 装備している武器のみレベルを描画
	if (isWeaponEquipped_[0]) {
		weaponLv1_->Draw();
		weaponLvFont1_->Draw();
	}
	if (isWeaponEquipped_[1]) {
		weaponLv2_->Draw();
		weaponLvFont2_->Draw();
	}
	if (isWeaponEquipped_[2]) {
		weaponLv3_->Draw();
		weaponLvFont3_->Draw();
	}
	if (isWeaponEquipped_[3]) {
		weaponLv4_->Draw();
		weaponLvFont4_->Draw();
	}

	if (isPaused_) {
		pauseBg_->Draw();
		back_->Draw();
		restart_->Draw();
		goTitle_->Draw();
	}
}

void GameSceneUI::DrawImGui() {
	//lvFont_->DrawImGui("LvFont");
	//lv_->DrawImGui("LvSprite");
	//moneyFont_->DrawImGui("NowMoneyFont");
	//money_->DrawImGui("MoneySprite");
	//enemyFont_->DrawImGui("EnemyFont");
	//enemy_->DrawImGui("EnemySprite");
	//expGauge_->DrawImGui("ExpGauge");
	//hpGauge_->DrawImGui("HpGauge");
	//currentHpFont_->DrawImGui("NowHpFont");
	//maxHpFont_->DrawImGui("MaxHpFont");
	//hpSrash_->DrawImGui("HpSrash");
	//weaponIcon1_->DrawImGui("WeaponIcon1");
	//weaponIcon2_->DrawImGui("WeaponIcon2");
	//weaponIcon3_->DrawImGui("WeaponIcon3");
	//weaponIcon4_->DrawImGui("WeaponIcon4");
	//playTimeFont_->DrawImGui("PlayTimeFont");
	//chestIcon_->DrawImGui("ChestIcon");
	//chestCostFont_->DrawImGui("ChestCostFont");
	//weaponLv1_->DrawImGui("lv1");
	//weaponLv2_->DrawImGui("lv2");
	//weaponLv3_->DrawImGui("lv3");
	//weaponLv4_->DrawImGui("lv4");
	//weaponLvFont1_->DrawImGui("lv1Font");
	//weaponLvFont2_->DrawImGui("lv2Font");
	//weaponLvFont3_->DrawImGui("lv3Font");
	//weaponLvFont4_->DrawImGui("lv4Font");
	//pauseBg_->DrawImGui("PauseBg");
	//back_->DrawImGui("Back");
	//restart_->DrawImGui("Restart");
	//goTitle_->DrawImGui("GoTitle");
}

void GameSceneUI::UpdateWeaponIcon(int slotIndex, WeaponName weaponName) {
	// スロット番号の検証
	if (slotIndex < 0 || slotIndex > 3) {
		return;
	}

	// 武器名からアイコンパスを取得
	std::string iconPath = GetWeaponIconPath(weaponName);

	// 対応するスロットのアイコンを更新
	switch (slotIndex) {
	case 0:
		weaponIcon1_->SetTexture(iconPath);
		break;
	case 1:
		weaponIcon2_->SetTexture(iconPath);
		break;
	case 2:
		weaponIcon3_->SetTexture(iconPath);
		break;
	case 3:
		weaponIcon4_->SetTexture(iconPath);
		break;
	}
}

std::string GameSceneUI::GetWeaponIconPath(WeaponName weaponName) const {
	switch (weaponName) {
	case WeaponName::FireBall:
		return "icon/fireball.png";
	case WeaponName::Laser:
		return "icon/laser.png";
	case WeaponName::Runa:
		return "icon/runa.png";
	case WeaponName::Axe:
		return "icon/axe.png";
	case WeaponName::Boomerang:
		return "icon/Boomerang.png";
	case WeaponName::Dice:
		return "icon/dice.png";
	case WeaponName::Toxic:
		return "icon/toxic.png";
	case WeaponName::Area:
		return "icon/area.png";
	case WeaponName::Gun:
		return "icon/gun.png";
	case WeaponName::None:
	default:
		return "icon/none.png";
	}
}