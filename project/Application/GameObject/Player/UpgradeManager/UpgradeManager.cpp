#include "UpgradeManager.h"
#include "GameObject/Player/WeaponManager/WeaponManager.h"
#include "Utility/Random/Random.h"

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

	upgradeName1_ = std::make_unique<Sprite>();
	upgradeName1_->Initialize("UI/Title/WeaponName/None.png");
	upgradeName1_->LoadFromJson("upgradeName1");
	upgradeName2_ = std::make_unique<Sprite>();
	upgradeName2_->Initialize("UI/Title/WeaponName/None.png");
	upgradeName2_->LoadFromJson("upgradeName2");
	upgradeName3_ = std::make_unique<Sprite>();
	upgradeName3_->Initialize("UI/Title/WeaponName/None.png");
	upgradeName3_->LoadFromJson("upgradeName3");

	newText1_ = std::make_unique<Sprite>();
	newText1_->Initialize("UI/game/newText.png");
	newText1_->LoadFromJson("newText1");
	newText2_ = std::make_unique<Sprite>();
	newText2_->Initialize("UI/game/newText.png");
	newText2_->LoadFromJson("newText2");
	newText3_ = std::make_unique<Sprite>();
	newText3_->Initialize("UI/game/newText.png");
	newText3_->LoadFromJson("newText3");

	upgradeText1_ = std::make_unique<Sprite>();
	upgradeText1_->Initialize("UI/game/upgradeText/testText.png");
	upgradeText1_->LoadFromJson("upgradeText1");
	upgradeText2_ = std::make_unique<Sprite>();
	upgradeText2_->Initialize("UI/game/upgradeText/testText.png");
	upgradeText2_->LoadFromJson("upgradeText2");
	upgradeText3_ = std::make_unique<Sprite>();
	upgradeText3_->Initialize("UI/game/upgradeText/testText.png");
	upgradeText3_->LoadFromJson("upgradeText3");

	isUpgrade_ = false;
}

void UpgradeManager::Update() {

	// アップグレード選択が有効でない場合は何もしない
	if (!isUpgrade_) {
		return;
	}

	// アップグレード画面が開かれた直後のフレームは入力を無視
	if (justOpened_) {
		justOpened_ = false;
		
		// UIの更新のみ行う
		UpdateUpgradeUI();
		bg_->Update();
		upgradeSelectText_->Update();
		upgradeBG1_->Update();
		upgradeBG2_->Update();
		upgradeBG3_->Update();
		upgradeIcon1_->Update();
		upgradeIcon2_->Update();
		upgradeIcon3_->Update();
		upgradeName1_->Update();
		upgradeName2_->Update();
		upgradeName3_->Update();
		newText1_->Update();
		newText2_->Update();
		newText3_->Update();
		upgradeText1_->Update();
		upgradeText2_->Update();
		upgradeText3_->Update();
		return;
	}

	switch (upgradeSelect_) {
	case UpgradeSelect::Select1:

		if (MyInput::Trigger(Action::CELECT_DOWN)) {
			upgradeSelect_ = UpgradeSelect::Select2;
		}

		// この選択肢の強化をしてゲームを再開
		if (MyInput::Trigger(Action::CONFIRM)) {
			ApplySelectedUpgrade();
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
			ApplySelectedUpgrade();
		 isUpgrade_ = false;
		}

		break;
	case UpgradeSelect::Select3:

		if (MyInput::Trigger(Action::CELECT_UP)) {
			upgradeSelect_ = UpgradeSelect::Select2;
		}

		// この選択肢の強化をしてゲームを再開
		if (MyInput::Trigger(Action::CONFIRM)) {
			ApplySelectedUpgrade();
		 isUpgrade_ = false;
		}

		break;
	}

	// 選択中のUIを強調表示
	UpdateUpgradeUI();

	bg_->Update();
	upgradeSelectText_->Update();
	upgradeBG1_->Update();
	upgradeBG2_->Update();
	upgradeBG3_->Update();
	upgradeIcon1_->Update();
	upgradeIcon2_->Update();
	upgradeIcon3_->Update();
	upgradeName1_->Update();
	upgradeName2_->Update();
	upgradeName3_->Update();
	newText1_->Update();
	newText2_->Update();
	newText3_->Update();
	upgradeText1_->Update();
	upgradeText2_->Update();
	upgradeText3_->Update();
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
		upgradeName1_->Draw();
		upgradeName2_->Draw();
		upgradeName3_->Draw();
		upgradeText1_->Draw();
		upgradeText2_->Draw();
		upgradeText3_->Draw();

		// NEW textは新規武器装備のときのみ描画
		if (upgradeOptions_[0].type == UpgradeType::NewWeapon) {
			newText1_->Draw();
		}
		if (upgradeOptions_[1].type == UpgradeType::NewWeapon) {
			newText2_->Draw();
		}
		if (upgradeOptions_[2].type == UpgradeType::NewWeapon) {
			newText3_->Draw();
		}
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
	//bg_->DrawImGui("UpgradeBGUI");
	//upgradeName1_->DrawImGui("UpgradeName1UI");
	//upgradeName2_->DrawImGui("UpgradeName2UI";
	//upgradeName3_->DrawImGui("UpgradeName3UI";
	//newText1_->DrawImGui("NewText1UI");
	//newText2_->DrawImGui("NewText2UI";
	//newText3_->DrawImGui("NewText3UI";
	//upgradeText1_->DrawImGui("UpgradeText1UI";
	//upgradeText2_->DrawImGui("UpgradeText2UI";
	//upgradeText3_->DrawImGui("UpgradeText3UI";
}

void UpgradeManager::Upgrade() {
	upgradeSelect_ = UpgradeSelect::Select1;
	isUpgrade_ = true;
	justOpened_ = true; // アップグレード画面が開かれた直後のフレームであることを記録
	
	// 3つのアップグレード選択肢を生成
	GenerateUpgradeOptions();

	// UIを更新（背景色とアイコン）
	// 新規武器の場合は白、強化の場合はレアリティ色
	upgradeBG1_->SetColor(upgradeOptions_[0].type == UpgradeType::NewWeapon ? 
		Vector4(0.75f, 0.75f, 0.75f, 1.0f) : GetRarityColor(upgradeOptions_[0].rarity));
	upgradeBG2_->SetColor(upgradeOptions_[1].type == UpgradeType::NewWeapon ? 
		Vector4(0.75f, 0.75f, 0.75f, 1.0f) : GetRarityColor(upgradeOptions_[1].rarity));
	upgradeBG3_->SetColor(upgradeOptions_[2].type == UpgradeType::NewWeapon ? 
		Vector4(0.75f, 0.75f, 0.75f, 1.0f) : GetRarityColor(upgradeOptions_[2].rarity));
	
	upgradeIcon1_->SetTexture(GetWeaponIconPath(upgradeOptions_[0].weaponName));
	upgradeIcon2_->SetTexture(GetWeaponIconPath(upgradeOptions_[1].weaponName));
	upgradeIcon3_->SetTexture(GetWeaponIconPath(upgradeOptions_[2].weaponName));
	
	// 武器名テクスチャを設定
	upgradeName1_->SetTexture(GetWeaponNamePath(upgradeOptions_[0].weaponName));
	upgradeName2_->SetTexture(GetWeaponNamePath(upgradeOptions_[1].weaponName));
	upgradeName3_->SetTexture(GetWeaponNamePath(upgradeOptions_[2].weaponName));
	
	// アップグレード説明テクスチャを設定
	upgradeText1_->SetTexture(GetUpgradeTextPath(upgradeOptions_[0].type, upgradeOptions_[0].rarity));
	upgradeText2_->SetTexture(GetUpgradeTextPath(upgradeOptions_[1].type, upgradeOptions_[1].rarity));
	upgradeText3_->SetTexture(GetUpgradeTextPath(upgradeOptions_[2].type, upgradeOptions_[2].rarity));
}

const UpgradeOption& UpgradeManager::GetSelectedOption() const {
	switch (upgradeSelect_) {
	case UpgradeSelect::Select1:
		return upgradeOptions_[0];
	case UpgradeSelect::Select2:
		return upgradeOptions_[1];
	case UpgradeSelect::Select3:
		return upgradeOptions_[2];
	default:
		return upgradeOptions_[0];
	}
}

Rarity UpgradeManager::RandomRarity() {
	// UnCommon 50%, Rare 37%, Epic 10%, Legendary 3%
	int roll = MyRand::Int(1, 100);
	
	if (roll <= 3) {
		return Rarity::Legendary;  // 3%
	} else if (roll <= 13) {
		return Rarity::Epic;       // 10%
	} else if (roll <= 50) {
		return Rarity::Rare;       // 37%
	} else {
		return Rarity::UnCommon;   // 50%
	}
}

void UpgradeManager::GenerateUpgradeOptions() {
	// 装備中の武器と未装備の武器を取得
	std::vector<WeaponName> equippedWeapons;
	std::vector<WeaponName> unequippedWeapons;
	
	if (weaponManager_) {
		const auto& weapons = weaponManager_->GetWeapons();
		
		// 装備中の武器を取得
		for (const auto& weapon : weapons) {
			WeaponName name = weapon->GetWeaponName();
			if (name != WeaponName::None) {
				equippedWeapons.push_back(name);
			}
		}
		
		// 未装備の武器を取得
		for (int i = static_cast<int>(WeaponName::FireBall); i < static_cast<int>(WeaponName::Count); ++i) {
			WeaponName name = static_cast<WeaponName>(i);
			if (!weaponManager_->HasWeapon(name)) {
				unequippedWeapons.push_back(name);
			}
		}
	}
	
	// 選択肢として既に選ばれた武器を追跡
	std::vector<WeaponName> selectedWeapons;
	
	// 3つの選択肢を生成
	for (int i = 0; i < 3; ++i) {
		UpgradeOption option;
		
		// まだ選択されていない武器のみを候補に
		std::vector<WeaponName> availableEquipped;
		std::vector<WeaponName> availableUnequipped;
		
		for (const auto& weapon : equippedWeapons) {
			bool alreadySelected = false;
			for (const auto& selected : selectedWeapons) {
				if (weapon == selected) {
					alreadySelected = true;
					break;
				}
			}
			if (!alreadySelected) {
				availableEquipped.push_back(weapon);
			}
		}
		
		for (const auto& weapon : unequippedWeapons) {
			bool alreadySelected = false;
			for (const auto& selected : selectedWeapons) {
				if (weapon == selected) {
					alreadySelected = true;
					break;
				}
			}
			if (!alreadySelected) {
				availableUnequipped.push_back(weapon);
			}
		}
		
		// 新規武器 or 既存武器強化を決定
		bool canEquipNew = !availableUnequipped.empty() && weaponManager_ && weaponManager_->CanEquipWeapon();
		bool canUpgrade = !availableEquipped.empty();
		
		if (canEquipNew && canUpgrade) {
			// 両方可能な場合はランダムに選択
			if (MyRand::Int(0, 1) == 0) {
				option.type = UpgradeType::NewWeapon;
				int index = MyRand::Int(0, static_cast<int>(availableUnequipped.size()) - 1);
				option.weaponName = availableUnequipped[index];
				// 新規武器の場合はRarityを設定しない（使用しない）
			} else {
				int index = MyRand::Int(0, static_cast<int>(availableEquipped.size()) - 1);
				option.weaponName = availableEquipped[index];
				
				// 武器に応じて利用可能な強化タイプを決定
				std::vector<UpgradeType> availableUpgradeTypes;
				availableUpgradeTypes.push_back(UpgradeType::UpgradeDamage);  // 全武器共通
				
				// Areaはサイズのみ（発射数なし）
				if (option.weaponName == WeaponName::Area) {
					availableUpgradeTypes.push_back(UpgradeType::UpgradeSize);
				}
				// Axe、Boomerang、Toxicはサイズと発射数の両方
				else if (option.weaponName == WeaponName::Axe || option.weaponName == WeaponName::Boomerang || 
						 option.weaponName == WeaponName::Toxic) {
					availableUpgradeTypes.push_back(UpgradeType::UpgradeSize);
					availableUpgradeTypes.push_back(UpgradeType::UpgradeShotMaxCount);
				}
				// それ以外の武器は発射数のみ
				else {
					availableUpgradeTypes.push_back(UpgradeType::UpgradeShotMaxCount);
				}
				
				// AxeとToxicの場合はUpgradeLifeTimeも追加
				if (option.weaponName == WeaponName::Axe || option.weaponName == WeaponName::Toxic) {
					availableUpgradeTypes.push_back(UpgradeType::UpgradeLifeTime);
				}
				
				// Laserの場合はUpgradePenetrationCountも追加
				if (option.weaponName == WeaponName::Laser) {
					availableUpgradeTypes.push_back(UpgradeType::UpgradePenetrationCount);
				}
				
				// Runaの場合はUpgradeBounceCountも追加
				if (option.weaponName == WeaponName::Runa) {
					availableUpgradeTypes.push_back(UpgradeType::UpgradeBounceCount);
				}
				
				// ランダムに強化タイプを選択
				int typeIndex = MyRand::Int(0, static_cast<int>(availableUpgradeTypes.size()) - 1);
				option.type = availableUpgradeTypes[typeIndex];
				option.rarity = RandomRarity();
			}
		} else if (canEquipNew) {
			option.type = UpgradeType::NewWeapon;
			int index = MyRand::Int(0, static_cast<int>(availableUnequipped.size()) - 1);
			option.weaponName = availableUnequipped[index];
			// 新規武器の場合はRarityを設定しない（使用しない）
		} else if (canUpgrade) {
			int index = MyRand::Int(0, static_cast<int>(availableEquipped.size()) - 1);
			option.weaponName = availableEquipped[index];
			
			// 武器に応じて利用可能な強化タイプを決定
			std::vector<UpgradeType> availableUpgradeTypes;
			availableUpgradeTypes.push_back(UpgradeType::UpgradeDamage);  // 全武器共通
			
			// Areaはサイズのみ（発射数なし）
			if (option.weaponName == WeaponName::Area) {
				availableUpgradeTypes.push_back(UpgradeType::UpgradeSize);
			}
			// Axe、Boomerang、Toxicはサイズと発射数の両方
			else if (option.weaponName == WeaponName::Axe || option.weaponName == WeaponName::Boomerang || 
					 option.weaponName == WeaponName::Toxic) {
				availableUpgradeTypes.push_back(UpgradeType::UpgradeSize);
				availableUpgradeTypes.push_back(UpgradeType::UpgradeShotMaxCount);
			}
			// それ以外の武器は発射数のみ
			else {
				availableUpgradeTypes.push_back(UpgradeType::UpgradeShotMaxCount);
			}
			
			// AxeとToxicの場合はUpgradeLifeTimeも追加
			if (option.weaponName == WeaponName::Axe || option.weaponName == WeaponName::Toxic) {
				availableUpgradeTypes.push_back(UpgradeType::UpgradeLifeTime);
			}
			
			// Laserの場合はUpgradePenetrationCountも追加
			if (option.weaponName == WeaponName::Laser) {
				availableUpgradeTypes.push_back(UpgradeType::UpgradePenetrationCount);
			}
			
			// Runaの場合はUpgradeBounceCountも追加
			if (option.weaponName == WeaponName::Runa) {
				availableUpgradeTypes.push_back(UpgradeType::UpgradeBounceCount);
			}
			
			// ランダムに強化タイプを選択
			int typeIndex = MyRand::Int(0, static_cast<int>(availableUpgradeTypes.size()) - 1);
			option.type = availableUpgradeTypes[typeIndex];
			option.rarity = RandomRarity();
		} else {
			// どちらもできない場合は何も設定しない（稀なケース）
			option.type = UpgradeType::UpgradeDamage;
			option.weaponName = WeaponName::None;
		}
		
		selectedWeapons.push_back(option.weaponName);
		upgradeOptions_[i] = option;
	}
}

Vector4 UpgradeManager::GetRarityColor(Rarity rarity) {
	switch (rarity) {
	case Rarity::UnCommon:
		return Vector4(0.2f, 0.8f, 0.2f, 1.0f);  // 緑
	case Rarity::Rare:
		return Vector4(0.2f, 0.4f, 1.0f, 1.0f);  // 青
	case Rarity::Epic:
		return Vector4(0.6f, 0.2f, 0.8f, 1.0f);  // 紫
	case Rarity::Legendary:
		return Vector4(1.0f, 0.6f, 0.0f, 1.0f);  // オレンジ
	default:
		return Vector4(0.75f, 0.75f, 0.75f, 1.0f);
	}
}

std::string UpgradeManager::GetWeaponIconPath(WeaponName weaponName) const {
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

std::string UpgradeManager::GetWeaponNamePath(WeaponName weaponName) const {
	switch (weaponName) {
	case WeaponName::FireBall:
		return "UI/Title/WeaponName/fireBall.png";
	case WeaponName::Laser:
		return "UI/Title/WeaponName/laser.png";
	case WeaponName::Runa:
		return "UI/Title/WeaponName/runa.png";
	case WeaponName::Axe:
		return "UI/Title/WeaponName/axe.png";
	case WeaponName::Boomerang:
		return "UI/Title/WeaponName/Boomerang.png";
	case WeaponName::Dice:
		return "UI/Title/WeaponName/dice.png";
	case WeaponName::Toxic:
		return "UI/Title/WeaponName/toxic.png";
	case WeaponName::Area:
		return "UI/Title/WeaponName/area.png";
	case WeaponName::Gun:
		return "UI/Title/WeaponName/gun.png";
	case WeaponName::None:
	default:
		return "UI/Title/WeaponName/None.png";
	}
}

std::string UpgradeManager::GetUpgradeTextPath(UpgradeType type, Rarity rarity) const {
	// 新規武器の場合は説明不要（空のパス、または共通テキスト）
	if (type == UpgradeType::NewWeapon) {
		return "UI/game/upgradeText/testText.png";
	}
	
	// レアリティ名を取得
	std::string rarityName;
	switch (rarity) {
	case Rarity::UnCommon:
		rarityName = "UnCommon";
		break;
	case Rarity::Rare:
		rarityName = "Rare";
		break;
	case Rarity::Epic:
		rarityName = "Epic";
		break;
	case Rarity::Legendary:
		rarityName = "Legendary";
		break;
	default:
		rarityName = "UnCommon";
		break;
	}
	
	// アップグレードタイプに応じてパスを生成
	if (type == UpgradeType::UpgradeDamage) {
		return "UI/game/upgradeText/Damage/" + rarityName + ".png";
	} else if (type == UpgradeType::UpgradeShotMaxCount) {
		return "UI/game/upgradeText/ShotMaxCount/" + rarityName + ".png";
	} else if (type == UpgradeType::UpgradeSize) {
		return "UI/game/upgradeText/Size/" + rarityName + ".png";
	} else if (type == UpgradeType::UpgradeLifeTime) {
		return "UI/game/upgradeText/LifeTime/" + rarityName + ".png";
	} else if (type == UpgradeType::UpgradePenetrationCount) {
		return "UI/game/upgradeText/PenetrationCount/" + rarityName + ".png";
	} else if (type == UpgradeType::UpgradeBounceCount) {
		return "UI/game/upgradeText/BounceCount/" + rarityName + ".png";
	}
	
	// デフォルト
	return "UI/game/upgradeText/testText.png";
}

void UpgradeManager::UpdateUpgradeUI() {
	// 選択中の背景を少し明るくする（強調表示）
	// 新規武器は白、強化はレアリティ色
	Vector4 color1 = upgradeOptions_[0].type == UpgradeType::NewWeapon ? 
		Vector4(0.75f, 0.75f, 0.75f, 1.0f) : GetRarityColor(upgradeOptions_[0].rarity);
	Vector4 color2 = upgradeOptions_[1].type == UpgradeType::NewWeapon ? 
		Vector4(0.75f, 0.75f, 0.75f, 1.0f) : GetRarityColor(upgradeOptions_[1].rarity);
	Vector4 color3 = upgradeOptions_[2].type == UpgradeType::NewWeapon ? 
		Vector4(0.75f, 0.75f, 0.75f, 1.0f) : GetRarityColor(upgradeOptions_[2].rarity);
	
	// 選択中でないものは少し暗くする
	float dimFactor = 0.6f;
	
	switch (upgradeSelect_) {
	case UpgradeSelect::Select1:
		upgradeBG1_->SetColor(color1);
		upgradeBG2_->SetColor(Vector4(color2.x * dimFactor, color2.y * dimFactor, color2.z * dimFactor, color2.w));
		upgradeBG3_->SetColor(Vector4(color3.x * dimFactor, color3.y * dimFactor, color3.z * dimFactor, color3.w));
		break;
	case UpgradeSelect::Select2:
		upgradeBG1_->SetColor(Vector4(color1.x * dimFactor, color1.y * dimFactor, color1.z * dimFactor, color1.w));
		upgradeBG2_->SetColor(color2);
		upgradeBG3_->SetColor(Vector4(color3.x * dimFactor, color3.y * dimFactor, color3.z * dimFactor, color3.w));
		break;
	case UpgradeSelect::Select3:
		upgradeBG1_->SetColor(Vector4(color1.x * dimFactor, color1.y * dimFactor, color1.z * dimFactor, color1.w));
		upgradeBG2_->SetColor(Vector4(color2.x * dimFactor, color2.y * dimFactor, color2.z * dimFactor, color2.w));
		upgradeBG3_->SetColor(color3);
		break;
	}
}

void UpgradeManager::ApplySelectedUpgrade() {
	if (!weaponManager_) {
		return;
	}
	
	const UpgradeOption& selected = GetSelectedOption();
	
	if (selected.weaponName == WeaponName::None) {
		return;
	}
	
	if (selected.type == UpgradeType::NewWeapon) {
		// 新しい武器を装備
		weaponManager_->EquipWeapon(selected.weaponName);
	} else if (selected.type == UpgradeType::UpgradeDamage) {
		// 既存の武器のダメージを強化
		const auto& weapons = weaponManager_->GetWeapons();
		for (auto& weapon : weapons) {
			if (weapon->GetWeaponName() == selected.weaponName) {
				weapon->UpgradeDamage(selected.rarity);
				break;
			}
		}
	} else if (selected.type == UpgradeType::UpgradeShotMaxCount) {
		// 既存の武器の発射数を強化
		const auto& weapons = weaponManager_->GetWeapons();
		for (auto& weapon : weapons) {
			if (weapon->GetWeaponName() == selected.weaponName) {
				weapon->UpgradeShotMaxCount(selected.rarity);
				break;
			}
		}
	} else if (selected.type == UpgradeType::UpgradeSize) {
		// 既存の武器のサイズを強化（Area、Toxic、Axe、Boomerang専用）
		const auto& weapons = weaponManager_->GetWeapons();
		for (auto& weapon : weapons) {
			if (weapon->GetWeaponName() == selected.weaponName) {
				weapon->UpgradeSize(selected.rarity);
				break;
			}
		}
	} else if (selected.type == UpgradeType::UpgradeLifeTime) {
		// 既存の武器の持続時間を強化（AxeとToxic専用）
		const auto& weapons = weaponManager_->GetWeapons();
		for (auto& weapon : weapons) {
			if (weapon->GetWeaponName() == selected.weaponName) {
				weapon->UpgradeLifeTime(selected.rarity);
				break;
			}
		}
	} else if (selected.type == UpgradeType::UpgradePenetrationCount) {
		// 既存の武器の貫通回数を強化（Laser専用）
		const auto& weapons = weaponManager_->GetWeapons();
		for (auto& weapon : weapons) {
			if (weapon->GetWeaponName() == selected.weaponName) {
				weapon->UpgradePenetrationCount(selected.rarity);
				break;
			}
		}
	} else if (selected.type == UpgradeType::UpgradeBounceCount) {
		// 既存の武器の反射回数を強化（Runa専用）
		const auto& weapons = weaponManager_->GetWeapons();
		for (auto& weapon : weapons) {
			if (weapon->GetWeaponName() == selected.weaponName) {
				weapon->UpgradeBounceCount(selected.rarity);
				break;
			}
		}
	}
}