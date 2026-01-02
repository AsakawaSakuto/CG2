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

	weaponIcon1_->Update();
	weaponIcon2_->Update();
	weaponIcon3_->Update();
	weaponIcon4_->Update();
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
	case WeaponName::None:
	default:
		return "icon/none.png";
	}
}