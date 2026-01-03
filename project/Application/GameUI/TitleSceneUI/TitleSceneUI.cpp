#include "TitleSceneUI.h"

void TitleSceneUI::Initialize() {
	play_ = std::make_unique<Sprite>();
	play_->Initialize("UI/Title/play.png");
	play_->LoadFromJson("PlayUI");

	quit_ = std::make_unique<Sprite>();
	quit_->Initialize("UI/Title/quit.png");
	quit_->LoadFromJson("QuitUI");

	edit_ = std::make_unique<Sprite>();
	edit_->Initialize("UI/Title/edit.png");
	edit_->LoadFromJson("EditUI");

	logo_ = std::make_unique<Sprite>();
	logo_->Initialize("UI/Title/gigabonk.png");

	ranking_ = std::make_unique<Sprite>();
	ranking_->Initialize("UI/Title/ranking.png");

	InitPlayerUI();

	InitWeaponUI();
}

void TitleSceneUI::Update() {

	switch (selectState_) {
	case TitleSelectState::Play:

		play_->SetScale(maxScale_);
		edit_->SetScale(minScale_);
		quit_->SetScale(minScale_);

		break;
	case TitleSelectState::Edit:

		play_->SetScale(minScale_);
		edit_->SetScale(maxScale_);
		quit_->SetScale(minScale_);

		break;
	case TitleSelectState::Quit:

		play_->SetScale(minScale_);
		edit_->SetScale(minScale_);
		quit_->SetScale(maxScale_);

		break;

	case  TitleSelectState::PlayerSelect:

		PlayerSelectUpdate();

		break;

	case  TitleSelectState::WeaponSelect:

		WeaponSelectUpdate();

		break;

	default:
		break;
	}

	play_->   Update();
	edit_->   Update();
	quit_->   Update();
	logo_->   Update();
	ranking_->Update();

	playerNameText_->Update();
	playerTypeText_->Update();
	powerManIcon_->Update();
	tankManIcon_->Update();
	jumpManIcon_->Update();
	speedManIcon_->Update();

	weaponNameText_->Update();
	weaponSelectText_->Update();
	fireBallIcon_->Update();
	laserIcon_->Update();
	runaIcon_->Update();
	axeIcon_->Update();
	boomerangIcon_->Update();
	diceIcon_->Update();
	toxicIcon_->Update();
	areaIcon_->Update();
	gunIcon_->Update();
}

void TitleSceneUI::Draw() {
	if (selectState_ != TitleSelectState::PlayerSelect && 
		selectState_ != TitleSelectState::WeaponSelect && 
		selectState_ != TitleSelectState::Confirmed) {
		play_->Draw();
		edit_->Draw();
		quit_->Draw();
		//logo_->Draw();
		//ranking_->Draw();
	} else {
		playerTypeText_->Draw();
		playerNameText_->Draw();
		powerManIcon_->Draw();
		tankManIcon_->Draw();
		jumpManIcon_->Draw();
		speedManIcon_->Draw();

		weaponSelectText_->Draw();
		weaponNameText_->Draw();
		fireBallIcon_->Draw();
		laserIcon_->Draw();
		runaIcon_->Draw();
		axeIcon_->Draw();
		boomerangIcon_->Draw();
		diceIcon_->Draw();
		toxicIcon_->Draw();
		areaIcon_->Draw();
		gunIcon_->Draw();
	}
}

void TitleSceneUI::DrawImGui() {
	//play_->DrawImGui("TitlePlayUI");
	//edit_->DrawImGui("TitleEditUI");
	//quit_->DrawImGui("TitleQuitUI");
	//logo_->DrawImGui("TitleLogoUI");
	//ranking_->DrawImGui("TitleRankingUI");
	//playerTypeText_->DrawImGui("PlayerTypeUI");
	//playerNameText_->DrawImGui("PlayerNameUI");
	//powerManIcon_->DrawImGui("PowerManIconUI");
	//tankManIcon_->DrawImGui("TankManIconUI");
	//jumpManIcon_->DrawImGui("JumpManIconUI");
	//speedManIcon_->DrawImGui("SpeedManIconUI");
	//weaponSelectText_->DrawImGui("WeaponSelectUI");
	//weaponNameText_->DrawImGui("WeaponNameUI");
	//fireBallIcon_->DrawImGui("FireBallIconUI");
	//laserIcon_->DrawImGui("LaserIconUI");
	//runaIcon_->DrawImGui("RunaIconUI");
	//axeIcon_->DrawImGui("AxeIconUI");
	//boomerangIcon_->DrawImGui("BoomerangIconUI");
	//diceIcon_->DrawImGui("DiceIconUI");
	//toxicIcon_->DrawImGui("ToxicIconUI");
	//areaIcon_->DrawImGui("AreaIconUI");
	//gunIcon_->DrawImGui("GunIconUI");
}

void TitleSceneUI::InitPlayerUI() {
	playerNameText_ = std::make_unique<Sprite>();
	playerNameText_->Initialize("UI/Title/playerName/PowerMan.png");
	playerNameText_->LoadFromJson("playerNameText");

	playerTypeText_ = std::make_unique<Sprite>();
	playerTypeText_->Initialize("UI/Title/playerType.png");
	playerTypeText_->LoadFromJson("playerTypeText");

	powerManIcon_ = std::make_unique<Sprite>();
	powerManIcon_->Initialize("Icon/PowerMan.png");
	powerManIcon_->LoadFromJson("powerManIconT");

	tankManIcon_ = std::make_unique<Sprite>();
	tankManIcon_->Initialize("Icon/TankMan.png");
	tankManIcon_->LoadFromJson("tankManIconT");

	jumpManIcon_ = std::make_unique<Sprite>();
	jumpManIcon_->Initialize("Icon/JumpMan.png");
	jumpManIcon_->LoadFromJson("jumpManIconT");

	speedManIcon_ = std::make_unique<Sprite>();
	speedManIcon_->Initialize("Icon/SpeedMan.png");
	speedManIcon_->LoadFromJson("speedManIconT");
}

void TitleSceneUI::PlayerSelectUpdate() {
	switch (playerName_) {
	case PlayerName::PowerMan:
		playerNameText_->SetTexture("UI/Title/playerName/PowerMan.png");
		powerManIcon_->SetScale(iconMax);
		tankManIcon_->SetScale(iconMin);
		jumpManIcon_->SetScale(iconMin);
		speedManIcon_->SetScale(iconMin);
		break;
	case PlayerName::TankMan:
		playerNameText_->SetTexture("UI/Title/playerName/TankMan.png");
		powerManIcon_->SetScale(iconMin);
		tankManIcon_->SetScale(iconMax);
		jumpManIcon_->SetScale(iconMin);
		speedManIcon_->SetScale(iconMin);
		break;
	case PlayerName::JumpMan:
		playerNameText_->SetTexture("UI/Title/playerName/JumpMan.png");
		powerManIcon_->SetScale(iconMin);
		tankManIcon_->SetScale(iconMin);
		jumpManIcon_->SetScale(iconMax);
		speedManIcon_->SetScale(iconMin);
		break;
	case PlayerName::SpeedMan:
		playerNameText_->SetTexture("UI/Title/playerName/SpeedMan.png");
		powerManIcon_->SetScale(iconMin);
		tankManIcon_->SetScale(iconMin);
		jumpManIcon_->SetScale(iconMin);
		speedManIcon_->SetScale(iconMax);
		break;
	default:
		break;
	}
}

void TitleSceneUI::InitWeaponUI() {
	weaponNameText_ = std::make_unique<Sprite>();
	weaponNameText_->Initialize("UI/Title/weaponName/fireBall.png");
	weaponNameText_->LoadFromJson("WeaponNameText");

	weaponSelectText_ = std::make_unique<Sprite>();
	weaponSelectText_->Initialize("UI/Title/WeaponSelect.png");
	weaponSelectText_->LoadFromJson("WeaponSelectText");

	fireBallIcon_ = std::make_unique<Sprite>();
	fireBallIcon_->Initialize("Icon/FireBall.png");
	fireBallIcon_->LoadFromJson("fireBallIconT");

	laserIcon_ = std::make_unique<Sprite>();
	laserIcon_->Initialize("Icon/Laser.png");
	laserIcon_->LoadFromJson("laserIconT");

	runaIcon_ = std::make_unique<Sprite>();
	runaIcon_->Initialize("Icon/Runa.png");
	runaIcon_->LoadFromJson("runaIconT");

	axeIcon_ = std::make_unique<Sprite>();
	axeIcon_->Initialize("Icon/Axe.png");
	axeIcon_->LoadFromJson("axeIconT");

	boomerangIcon_ = std::make_unique<Sprite>();
	boomerangIcon_->Initialize("Icon/Boomerang.png");
	boomerangIcon_->LoadFromJson("boomerangIconT");

	diceIcon_ = std::make_unique<Sprite>();
	diceIcon_->Initialize("Icon/dice.png");
	diceIcon_->LoadFromJson("diceIconT");

	toxicIcon_ = std::make_unique<Sprite>();
	toxicIcon_->Initialize("Icon/Toxic.png");
	toxicIcon_->LoadFromJson("toxicIconT");

	areaIcon_ = std::make_unique<Sprite>();
	areaIcon_->Initialize("Icon/Area.png");
	areaIcon_->LoadFromJson("areaIconT");

	gunIcon_ = std::make_unique<Sprite>();
	gunIcon_->Initialize("Icon/Gun.png");
	gunIcon_->LoadFromJson("gunIconT");

	weaponNameText_->SetTexture("UI/Title/weaponName/fireBall.png");
	fireBallIcon_->SetScale(iconMax);
	laserIcon_->SetScale(iconMin);
	runaIcon_->SetScale(iconMin);
	axeIcon_->SetScale(iconMin);
	boomerangIcon_->SetScale(iconMin);
	diceIcon_->SetScale(iconMin);
	toxicIcon_->SetScale(iconMin);
	areaIcon_->SetScale(iconMin);
	gunIcon_->SetScale(iconMin);
}

void TitleSceneUI::WeaponSelectUpdate() {
	switch (weaponName_) {
	case WeaponName::FireBall:
		weaponNameText_->SetTexture("UI/Title/weaponName/fireBall.png");
		fireBallIcon_->SetScale(iconMax);
		laserIcon_->SetScale(iconMin);
		runaIcon_->SetScale(iconMin);
		axeIcon_->SetScale(iconMin);
		boomerangIcon_->SetScale(iconMin);
		diceIcon_->SetScale(iconMin);
		toxicIcon_->SetScale(iconMin);
		areaIcon_->SetScale(iconMin);
		gunIcon_->SetScale(iconMin);
		break;
	case WeaponName::Laser:
		weaponNameText_->SetTexture("UI/Title/weaponName/laser.png");
		fireBallIcon_->SetScale(iconMin);
		laserIcon_->SetScale(iconMax);
		runaIcon_->SetScale(iconMin);
		axeIcon_->SetScale(iconMin);
		boomerangIcon_->SetScale(iconMin);
		diceIcon_->SetScale(iconMin);
		toxicIcon_->SetScale(iconMin);
		areaIcon_->SetScale(iconMin);
		gunIcon_->SetScale(iconMin);
		break;
	case WeaponName::Runa:
		weaponNameText_->SetTexture("UI/Title/weaponName/runa.png");
		fireBallIcon_->SetScale(iconMin);
		laserIcon_->SetScale(iconMin);
		runaIcon_->SetScale(iconMax);
		axeIcon_->SetScale(iconMin);
		boomerangIcon_->SetScale(iconMin);
		diceIcon_->SetScale(iconMin);
		toxicIcon_->SetScale(iconMin);
		areaIcon_->SetScale(iconMin);
		gunIcon_->SetScale(iconMin);
		break;
	case WeaponName::Axe:
		weaponNameText_->SetTexture("UI/Title/weaponName/axe.png");
		fireBallIcon_->SetScale(iconMin);
		laserIcon_->SetScale(iconMin);
		runaIcon_->SetScale(iconMin);
		axeIcon_->SetScale(iconMax);
		boomerangIcon_->SetScale(iconMin);
		diceIcon_->SetScale(iconMin);
		toxicIcon_->SetScale(iconMin);
		areaIcon_->SetScale(iconMin);
		gunIcon_->SetScale(iconMin);
		break;
	case WeaponName::Boomerang:
		weaponNameText_->SetTexture("UI/Title/weaponName/Boomerang.png");
		fireBallIcon_->SetScale(iconMin);
		laserIcon_->SetScale(iconMin);
		runaIcon_->SetScale(iconMin);
		axeIcon_->SetScale(iconMin);
		boomerangIcon_->SetScale(iconMax);
		diceIcon_->SetScale(iconMin);
		toxicIcon_->SetScale(iconMin);
		areaIcon_->SetScale(iconMin);
		gunIcon_->SetScale(iconMin);
		break;
	case WeaponName::Dice:
		weaponNameText_->SetTexture("UI/Title/weaponName/dice.png");
		fireBallIcon_->SetScale(iconMin);
		laserIcon_->SetScale(iconMin);
		runaIcon_->SetScale(iconMin);
		axeIcon_->SetScale(iconMin);
		boomerangIcon_->SetScale(iconMin);
		diceIcon_->SetScale(iconMax);
		toxicIcon_->SetScale(iconMin);
		areaIcon_->SetScale(iconMin);
		gunIcon_->SetScale(iconMin);
		break;
	case WeaponName::Toxic:
		weaponNameText_->SetTexture("UI/Title/weaponName/toxic.png");
		fireBallIcon_->SetScale(iconMin);
		laserIcon_->SetScale(iconMin);
		runaIcon_->SetScale(iconMin);
		axeIcon_->SetScale(iconMin);
		boomerangIcon_->SetScale(iconMin);
		diceIcon_->SetScale(iconMin);
		toxicIcon_->SetScale(iconMax);
		areaIcon_->SetScale(iconMin);
		gunIcon_->SetScale(iconMin);
		break;
	case WeaponName::Area:
		weaponNameText_->SetTexture("UI/Title/weaponName/area.png");
		fireBallIcon_->SetScale(iconMin);
		laserIcon_->SetScale(iconMin);
		runaIcon_->SetScale(iconMin);
		axeIcon_->SetScale(iconMin);
		boomerangIcon_->SetScale(iconMin);
		diceIcon_->SetScale(iconMin);
		toxicIcon_->SetScale(iconMin);
		areaIcon_->SetScale(iconMax);
		gunIcon_->SetScale(iconMin);
		break;
	case WeaponName::Gun:
		weaponNameText_->SetTexture("UI/Title/weaponName/gun.png");
		fireBallIcon_->SetScale(iconMin);
		laserIcon_->SetScale(iconMin);
		runaIcon_->SetScale(iconMin);
		axeIcon_->SetScale(iconMin);
		boomerangIcon_->SetScale(iconMin);
		diceIcon_->SetScale(iconMin);
		toxicIcon_->SetScale(iconMin);
		areaIcon_->SetScale(iconMin);
		gunIcon_->SetScale(iconMax);
		break;
	case WeaponName::Count:
		break;
	default:
		break;
	}
}