#pragma once
#include "GameUI/BaseUI.h"
#include "EngineSystem.h"
#include"GameObject/Player/playerStatus.h"
#include"GameObject/Player/WeaponManager/WeaponStatus.h"

enum class TitleSelectState {
	Play,
	Edit,
	Quit,
	PlayerSelect,
	WeaponSelect,
	Confirmed,

	Count
};

class TitleSceneUI : public BaseUI {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;

	void SetSelectState(TitleSelectState state) { selectState_ = state; }
	void SetPlayerName(PlayerName name) { playerName_ = name; }
	void SetWeaponName(WeaponName name) { weaponName_ = name; }
private:

	TitleSelectState selectState_ = TitleSelectState::Play;

	PlayerName playerName_ = PlayerName::PowerMan;
	WeaponName weaponName_ = WeaponName::FireBall;

	void InitPlayerUI();
	void InitWeaponUI();

	void PlayerSelectUpdate();
	void WeaponSelectUpdate();
private:

	unique_ptr<Sprite> play_;
	unique_ptr<Sprite> edit_;
	unique_ptr<Sprite> quit_;
	unique_ptr<Sprite> logo_;
	unique_ptr<Sprite> ranking_;

	// PlayerSelectUI
	unique_ptr<Sprite> playerTypeText_;
	unique_ptr<Sprite> playerNameText_;

	unique_ptr<Sprite> powerManIcon_;
	unique_ptr<Sprite> tankManIcon_;
	unique_ptr<Sprite> jumpManIcon_;
	unique_ptr<Sprite> speedManIcon_;

	// WeaponSelectUI
	unique_ptr<Sprite> weaponSelectText_;
	unique_ptr<Sprite> weaponNameText_;

	unique_ptr<Sprite> fireBallIcon_;
	unique_ptr<Sprite> laserIcon_;
	unique_ptr<Sprite> runaIcon_;
	unique_ptr<Sprite> axeIcon_;
	unique_ptr<Sprite> boomerangIcon_;
	unique_ptr<Sprite> diceIcon_;
	unique_ptr<Sprite> toxicIcon_;
	unique_ptr<Sprite> areaIcon_;
	unique_ptr<Sprite> gunIcon_;

	// Scale
	Vector2 maxScale_ = { 0.8f,0.8f };
	Vector2 minScale_ = { 0.5f,0.5f };
	Vector2 iconMin = { 0.9f,0.9f };
	Vector2 iconMax = { 1.1f,1.1f };
};