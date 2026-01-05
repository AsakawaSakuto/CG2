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

	confirmed_ = std::make_unique<Sprite>();
	confirmed_->Initialize("UI/Title/confirmed.png");
	confirmed_->LoadFromJson("ConfirmedText");
	confirmedBG_ = std::make_unique<Sprite>();
	confirmedBG_->Initialize("UI/Title/confirmedBG.png");
	confirmedBG_->LoadFromJson("ConfBG");

	InitPlayerUI();

	InitWeaponUI();

	scaleTimer_.Start(1.0f, true);
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

	case TitleSelectState::PlayerSelect:

		PlayerSelectUpdate();

		break;

	case TitleSelectState::WeaponSelect:

		WeaponSelectUpdate();

		break;

	case TitleSelectState::Confirmed:

		playerTypeText_->SetScale(textMin_);
		weaponSelectText_->SetScale(textMin_);
		confirmed_->SetScale(MyEasing::Lerp_GAB(confMin_, confMax_, scaleTimer_.GetProgress()));

		break;

	default:
		break;
	}

	scaleTimer_.Update();

	play_->   Update();
	edit_->   Update();
	quit_->   Update();
	logo_->   Update();
	ranking_->Update();

	playerNameText_->Update();
	playerTypeText_->Update();
	playerAbilityText_->Update();
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

	confirmed_->Update();
	confirmedBG_->Update();
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
		playerAbilityText_->Draw();
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

	if (selectState_ == TitleSelectState::Confirmed) {
		confirmedBG_->Draw();
		confirmed_->Draw();
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
	//Confirmed_->DrawImGui("ConfirmedUI");]
	//confirmedBG_->DrawImGui("ConfirmedBGUI");
	playerAbilityText_->DrawImGui("PlayerAbilityUI");
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
	jumpManIcon_->Initialize("Icon/jumpMan.png");
	jumpManIcon_->LoadFromJson("jumpManIconT");

	speedManIcon_ = std::make_unique<Sprite>();
	speedManIcon_->Initialize("Icon/SpeedMan.png");
	speedManIcon_->LoadFromJson("speedManIconT");

	playerAbilityText_ = std::make_unique<Sprite>();
	playerAbilityText_->Initialize("UI/Title/playerAbilityText/test.png");
	playerAbilityText_->LoadFromJson("PlayerAbilityText");
}

void TitleSceneUI::PlayerSelectUpdate() {
	switch (playerName_) {
	case PlayerName::PowerMan:
		playerNameText_->SetTexture("UI/Title/playerName/PowerMan.png");
		playerAbilityText_->SetTexture("UI/Title/playerAbilityText/PowerMan.png");
		powerManIcon_->SetScale(iconMax_);
		tankManIcon_->SetScale(iconMin_);
		jumpManIcon_->SetScale(iconMin_);
		speedManIcon_->SetScale(iconMin_);
		break;
	case PlayerName::TankMan:
		playerNameText_->SetTexture("UI/Title/playerName/TankMan.png");
		playerAbilityText_->SetTexture("UI/Title/playerAbilityText/TankMan.png");
		powerManIcon_->SetScale(iconMin_);
		tankManIcon_->SetScale(iconMax_);
		jumpManIcon_->SetScale(iconMin_);
		speedManIcon_->SetScale(iconMin_);
		break;
	case PlayerName::JumpMan:
		playerNameText_->SetTexture("UI/Title/playerName/jumpMan.png");
		playerAbilityText_->SetTexture("UI/Title/playerAbilityText/JumpMan.png");
		powerManIcon_->SetScale(iconMin_);
		tankManIcon_->SetScale(iconMin_);
		jumpManIcon_->SetScale(iconMax_);
		speedManIcon_->SetScale(iconMin_);
		break;
	case PlayerName::SpeedMan:
		playerNameText_->SetTexture("UI/Title/playerName/SpeedMan.png");
		playerAbilityText_->SetTexture("UI/Title/playerAbilityText/SpeedMan.png");
		powerManIcon_->SetScale(iconMin_);
		tankManIcon_->SetScale(iconMin_);
		jumpManIcon_->SetScale(iconMin_);
		speedManIcon_->SetScale(iconMax_);
		break;
	default:
		break;
	}

	playerTypeText_->SetScale(MyEasing::Lerp_GAB(textMin_, textMax_, scaleTimer_.GetProgress()));
	weaponSelectText_->SetScale(textMin_);
	confirmed_->SetScale(confMin_);
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
	fireBallIcon_->SetScale(iconMax_);
	laserIcon_->SetScale(iconMin_);
	runaIcon_->SetScale(iconMin_);
	axeIcon_->SetScale(iconMin_);
	boomerangIcon_->SetScale(iconMin_);
	diceIcon_->SetScale(iconMin_);
	toxicIcon_->SetScale(iconMin_);
	areaIcon_->SetScale(iconMin_);
	gunIcon_->SetScale(iconMin_);
}

void TitleSceneUI::WeaponSelectUpdate() {
	switch (weaponName_) {
	case WeaponName::FireBall:
		weaponNameText_->SetTexture("UI/Title/weaponName/fireBall.png");
		fireBallIcon_->SetScale(iconMax_);
		laserIcon_->SetScale(iconMin_);
		runaIcon_->SetScale(iconMin_);
		axeIcon_->SetScale(iconMin_);
		boomerangIcon_->SetScale(iconMin_);
		diceIcon_->SetScale(iconMin_);
		toxicIcon_->SetScale(iconMin_);
		areaIcon_->SetScale(iconMin_);
		gunIcon_->SetScale(iconMin_);
		break;
	case WeaponName::Laser:
		weaponNameText_->SetTexture("UI/Title/weaponName/laser.png");
		fireBallIcon_->SetScale(iconMin_);
		laserIcon_->SetScale(iconMax_);
		runaIcon_->SetScale(iconMin_);
		axeIcon_->SetScale(iconMin_);
		boomerangIcon_->SetScale(iconMin_);
		diceIcon_->SetScale(iconMin_);
		toxicIcon_->SetScale(iconMin_);
		areaIcon_->SetScale(iconMin_);
		gunIcon_->SetScale(iconMin_);
		break;
	case WeaponName::Runa:
		weaponNameText_->SetTexture("UI/Title/weaponName/runa.png");
		fireBallIcon_->SetScale(iconMin_);
		laserIcon_->SetScale(iconMin_);
		runaIcon_->SetScale(iconMax_);
		axeIcon_->SetScale(iconMin_);
		boomerangIcon_->SetScale(iconMin_);
		diceIcon_->SetScale(iconMin_);
		toxicIcon_->SetScale(iconMin_);
		areaIcon_->SetScale(iconMin_);
		gunIcon_->SetScale(iconMin_);
		break;
	case WeaponName::Axe:
		weaponNameText_->SetTexture("UI/Title/weaponName/axe.png");
		fireBallIcon_->SetScale(iconMin_);
		laserIcon_->SetScale(iconMin_);
		runaIcon_->SetScale(iconMin_);
		axeIcon_->SetScale(iconMax_);
		boomerangIcon_->SetScale(iconMin_);
		diceIcon_->SetScale(iconMin_);
		toxicIcon_->SetScale(iconMin_);
		areaIcon_->SetScale(iconMin_);
		gunIcon_->SetScale(iconMin_);
		break;
	case WeaponName::Boomerang:
		weaponNameText_->SetTexture("UI/Title/weaponName/Boomerang.png");
		fireBallIcon_->SetScale(iconMin_);
		laserIcon_->SetScale(iconMin_);
		runaIcon_->SetScale(iconMin_);
		axeIcon_->SetScale(iconMin_);
		boomerangIcon_->SetScale(iconMax_);
		diceIcon_->SetScale(iconMin_);
		toxicIcon_->SetScale(iconMin_);
		areaIcon_->SetScale(iconMin_);
		gunIcon_->SetScale(iconMin_);
		break;
	case WeaponName::Dice:
		weaponNameText_->SetTexture("UI/Title/weaponName/dice.png");
		fireBallIcon_->SetScale(iconMin_);
		laserIcon_->SetScale(iconMin_);
		runaIcon_->SetScale(iconMin_);
		axeIcon_->SetScale(iconMin_);
		boomerangIcon_->SetScale(iconMin_);
		diceIcon_->SetScale(iconMax_);
		toxicIcon_->SetScale(iconMin_);
		areaIcon_->SetScale(iconMin_);
		gunIcon_->SetScale(iconMin_);
		break;
	case WeaponName::Toxic:
		weaponNameText_->SetTexture("UI/Title/weaponName/toxic.png");
		fireBallIcon_->SetScale(iconMin_);
		laserIcon_->SetScale(iconMin_);
		runaIcon_->SetScale(iconMin_);
		axeIcon_->SetScale(iconMin_);
		boomerangIcon_->SetScale(iconMin_);
		diceIcon_->SetScale(iconMin_);
		toxicIcon_->SetScale(iconMax_);
		areaIcon_->SetScale(iconMin_);
		gunIcon_->SetScale(iconMin_);
		break;
	case WeaponName::Area:
		weaponNameText_->SetTexture("UI/Title/weaponName/area.png");
		fireBallIcon_->SetScale(iconMin_);
		laserIcon_->SetScale(iconMin_);
		runaIcon_->SetScale(iconMin_);
		axeIcon_->SetScale(iconMin_);
		boomerangIcon_->SetScale(iconMin_);
		diceIcon_->SetScale(iconMin_);
		toxicIcon_->SetScale(iconMin_);
		areaIcon_->SetScale(iconMax_);
		gunIcon_->SetScale(iconMin_);
		break;
	case WeaponName::Gun:
		weaponNameText_->SetTexture("UI/Title/weaponName/gun.png");
		fireBallIcon_->SetScale(iconMin_);
		laserIcon_->SetScale(iconMin_);
		runaIcon_->SetScale(iconMin_);
		axeIcon_->SetScale(iconMin_);
		boomerangIcon_->SetScale(iconMin_);
		diceIcon_->SetScale(iconMin_);
		toxicIcon_->SetScale(iconMin_);
		areaIcon_->SetScale(iconMin_);
		gunIcon_->SetScale(iconMax_);
		break;
	case WeaponName::Count:
		break;
	default:
		break;
	}

	weaponSelectText_->SetScale(MyEasing::Lerp_GAB(textMin_, textMax_, scaleTimer_.GetProgress()));
	playerTypeText_->SetScale(textMin_);
	confirmed_->SetScale(confMin_);
}