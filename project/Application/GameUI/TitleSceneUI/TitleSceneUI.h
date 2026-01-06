#pragma once
#include "GameUI/BaseUI.h"
#include "EngineSystem.h"
#include"GameObject/Player/playerStatus.h"
#include"GameObject/Player/WeaponManager/WeaponStatus.h"

enum class TitleSelectState {
	Play,
	Edit,
	EditSelect,
	Quit,
	PlayerSelect,
	WeaponSelect,
	Confirmed,

	Count
};

enum class EditType {
	Screen,
	BgmVolume,
	SeVolume,

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
	void SetEditType(EditType type) { editType_ = type; }
	void SetVolume(int bgm, int se) { 
		bgmValueFont_->SetNumber(bgm);
		seValueFont_->SetNumber(se);
	}
	void SetIsFullScreen(bool isFull) { if (isFull) { onOff_->SetTexture("UI/title/on.png"); } else { onOff_->SetTexture("UI/title/off.png"); } }
	void SetRanking(int rank1st, int rank2nd, int rank3rd) {
		ranking1st_->SetNumber(rank1st);
		ranking2nd_->SetNumber(rank2nd);
		ranking3rd_->SetNumber(rank3rd);
	}
private:

	TitleSelectState selectState_ = TitleSelectState::Play;

	PlayerName playerName_ = PlayerName::PowerMan;
	WeaponName weaponName_ = WeaponName::FireBall;
	EditType editType_ = EditType::Screen;

	void InitPlayerUI();
	void InitWeaponUI();
	void InitEditUI();

	void PlayerSelectUpdate();
	void WeaponSelectUpdate();
private:

	// editUI
	unique_ptr<Sprite> fullScreen_;
	unique_ptr<Sprite> onOff_;
	unique_ptr<Sprite> bgmVolume_;
	unique_ptr<Sprite> seVolume_;
	unique_ptr<BitmapFont> bgmValueFont_;
	unique_ptr<BitmapFont> seValueFont_;
	unique_ptr<Sprite> editBG_;
	Vector2 editMin_ = { 0.4f,0.4f };
	Vector2 editMax_ = { 0.5f,0.5f };

	unique_ptr<Sprite> select1_;
	unique_ptr<Sprite> select2_;
	unique_ptr<Sprite> select3_;
	unique_ptr<Sprite> select4_;
	unique_ptr<Sprite> select5_;
	unique_ptr<Sprite> select6_;

	GameTimer scaleTimer_;

	unique_ptr<Sprite> play_;
	unique_ptr<Sprite> edit_;
	unique_ptr<Sprite> quit_;
	unique_ptr<Sprite> logo_;
	unique_ptr<Sprite> rankingBG_;
	unique_ptr<BitmapFont> ranking1st_;
	unique_ptr<BitmapFont> ranking2nd_;
	unique_ptr<BitmapFont> ranking3rd_;

	// PlayerSelectUI
	unique_ptr<Sprite> playerTypeText_;
	unique_ptr<Sprite> playerNameText_;
	unique_ptr<Sprite> playerAbilityText_;

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

	//
	unique_ptr<Sprite> confirmed_;
	unique_ptr<Sprite> confirmedBG_;

	// Scale
	Vector2 maxScale_ = { 0.5f,0.5f };
	Vector2 minScale_ = { 0.3f,0.3f };
	Vector2 iconMin_ = { 0.9f,0.9f };
	Vector2 iconMax_ = { 1.1f,1.1f };
	Vector2 textMin_ = { 0.2f,0.2f };
	Vector2 textMax_ = { 0.22f,0.22f };
	Vector2 confMin_ = { 0.45f,0.45f };
	Vector2 confMax_ = { 0.5f,0.5f };
};