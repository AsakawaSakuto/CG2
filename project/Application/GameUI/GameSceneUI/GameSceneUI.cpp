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

	hpSlash_ = std::make_unique<Sprite>();
	hpSlash_->Initialize("bitMapFont/srash.png", { 137.0f, 65.0f }, { 0.2f, 0.2f });
	hpSlash_->SetAnchorPoint(AnchorPoint::Center);

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

	startText_ = std::make_unique<Sprite>();
	startText_->Initialize("UI/game/startText.png");
	startText_->SetAnchorPoint(AnchorPoint::Center);

	resultBg_ = std::make_unique<Sprite>();
	resultBg_->Initialize("UI/Game/resultBg.png", { 640.0f,360.0f });
	resultBg_->SetAnchorPoint(AnchorPoint::Center);
	resultBg_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultRestart_ = make_unique<Sprite>();
	resultRestart_->Initialize("UI/Game/Restart.png");
	resultRestart_->LoadFromJson("resultRestart");
	resultRestart_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultQuit_ = make_unique<Sprite>();
	resultQuit_->Initialize("UI/Game/titleText.png");
	resultQuit_->LoadFromJson("resultQuit");
	resultQuit_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultEnemyIcon_ = std::make_unique<Sprite>();
	resultEnemyIcon_->Initialize("ui/game/enemy.png");
	resultEnemyIcon_->LoadFromJson("resultEnemy");
	resultEnemyIcon_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultWeaponIcon1_ = std::make_unique<Sprite>();
	resultWeaponIcon2_ = std::make_unique<Sprite>();
	resultWeaponIcon3_ = std::make_unique<Sprite>();
	resultWeaponIcon4_ = std::make_unique<Sprite>();
	resultWeaponIcon1_->Initialize("icon/none.png");
	resultWeaponIcon2_->Initialize("icon/none.png");
	resultWeaponIcon3_->Initialize("icon/none.png");
	resultWeaponIcon4_->Initialize("icon/none.png");
	resultWeaponIcon1_->LoadFromJson("resultWeapon1");
	resultWeaponIcon2_->LoadFromJson("resultWeapon2");
	resultWeaponIcon3_->LoadFromJson("resultWeapon3");
	resultWeaponIcon4_->LoadFromJson("resultWeapon4");
	resultWeaponIcon1_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultWeaponIcon2_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultWeaponIcon3_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultWeaponIcon4_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultKillEnemyFont_ = std::make_unique<BitmapFont>();
	resultKillEnemyFont_->Initialize("resultAllkill");
	resultKillEnemyFont_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultKillEnemyWeaponFont1_ = std::make_unique<BitmapFont>();
	resultKillEnemyWeaponFont2_ = std::make_unique<BitmapFont>();
	resultKillEnemyWeaponFont3_ = std::make_unique<BitmapFont>();
	resultKillEnemyWeaponFont4_ = std::make_unique<BitmapFont>();
	resultKillEnemyWeaponFont1_->Initialize("resultWeaponFont1");
	resultKillEnemyWeaponFont2_->Initialize("resultWeaponFont2");
	resultKillEnemyWeaponFont3_->Initialize("resultWeaponFont3");
	resultKillEnemyWeaponFont4_->Initialize("resultWeaponFont4");
	resultKillEnemyWeaponFont1_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultKillEnemyWeaponFont2_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultKillEnemyWeaponFont3_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	resultKillEnemyWeaponFont4_->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	text_ = std::make_unique<Sprite>();
	text_->Initialize("UI/game/Text.png", { 640.0f,720.0f - 32.0f });
	text_->SetAnchorPoint(AnchorPoint::Center);
	text2_ = std::make_unique<Sprite>();
	text2_->Initialize("UI/game/Text2.png", { 640.0f,720.0f - 96.0f });
	text2_->SetAnchorPoint(AnchorPoint::Center);

	textPad_ = std::make_unique<Sprite>();
	textPad_->Initialize("UI/game/TextPad.png", { 5.0f,415.0f });
	
	isTextDraw_ = true;
	
	last_ = make_unique<Sprite>();
	last_->Initialize("UI/game/last.png", { -500.0f,360.0f });
	last_->SetAnchorPoint(AnchorPoint::Center);
	isLastDraw_ = false;

	startTimer_.Start(2.0f, false);

	minMap_ = std::make_unique<Sprite>();
	minMap_->Initialize("UI/game/minMap/Map.png");
	minMap_->LoadFromJson("minMap");

	mapPlayer_ = std::make_unique<Sprite>();
	mapPlayer_->Initialize("UI/game/minMap/player.png");
	mapPlayer_->SetAnchorPoint(AnchorPoint::Center);
}

void GameSceneUI::UpdateMapPlayerPosition() {
	// ワールド座標をミニマップのスクリーン座標に変換
	Vector2 screenPos = WorldToMapScreen(playerWorldPosition_);
	
	// スプライトの位置を更新
	mapPlayer_->SetPosition(screenPos);
	
	// スプライトの回転を更新
	mapPlayer_->SetRotate(playerRotationY_);
}

Vector2 GameSceneUI::WorldToMapScreen(const Vector3& worldPos) const {
	// X軸の変換（線形補間）
	float normalizedX = (worldPos.x - mapWorldRight_) / (mapWorldLeft_ - mapWorldRight_);
	float screenX = mapScreenLeft_ + normalizedX * (mapScreenRight_ - mapScreenLeft_);
	
	// Z軸の変換（線形補間）
	float normalizedZ = (worldPos.z - mapWorldBottom_) / (mapWorldTop_ - mapWorldBottom_);
	float screenY = mapScreenBottom_ + normalizedZ * (mapScreenTop_ - mapScreenBottom_);
	
	// ミニマップの範囲内にクランプ
	screenX = std::clamp(screenX, mapScreenLeft_, mapScreenRight_);
	screenY = std::clamp(screenY, mapScreenTop_, mapScreenBottom_);
	
	return { screenX, screenY };
}

void GameSceneUI::UpdateChestIcons(const std::vector<Vector3>& chestPositions) {
	// アイコン数が変わった場合のみ再生成
	if (mapChestIcons_.size() != chestPositions.size()) {
		// 既存のアイコンをクリア
		mapChestIcons_.clear();
		
		// 各Chestの位置にアイコンを作成
		for (const auto& pos : chestPositions) {
			auto icon = std::make_unique<Sprite>();
			icon->Initialize(chestIconPath_);
			icon->SetAnchorPoint(AnchorPoint::Center);
			
			// ワールド座標をスクリーン座標に変換
			Vector2 screenPos = WorldToMapScreen(pos);
			icon->SetPosition(screenPos);
			
			mapChestIcons_.push_back(std::move(icon));
		}
	}
}

void GameSceneUI::UpdateJarIcons(const std::vector<Vector3>& expJarPositions, 
                                 const std::vector<Vector3>& moneyJarPositions) {
	// ExpJarアイコン数が変わった場合のみ再生成
	if (mapExpJarIcons_.size() != expJarPositions.size()) {
		mapExpJarIcons_.clear();
		
		// ExpJarのアイコンを作成
		for (const auto& pos : expJarPositions) {
			auto icon = std::make_unique<Sprite>();
			icon->Initialize(expJarIconPath_);
			icon->SetAnchorPoint(AnchorPoint::Center);
			
			// ワールド座標をスクリーン座標に変換
			Vector2 screenPos = WorldToMapScreen(pos);
			icon->SetPosition(screenPos);
			
			mapExpJarIcons_.push_back(std::move(icon));
		}
	}
	
	// MoneyJarアイコン数が変わった場合のみ再生成
	if (mapMoneyJarIcons_.size() != moneyJarPositions.size()) {
		mapMoneyJarIcons_.clear();
		
		// MoneyJarのアイコンを作成
		for (const auto& pos : moneyJarPositions) {
			auto icon = std::make_unique<Sprite>();
			icon->Initialize(moneyJarIconPath_);
			icon->SetAnchorPoint(AnchorPoint::Center);
			
			// ワールド座標をスクリーン座標に変換
			Vector2 screenPos = WorldToMapScreen(pos);
			icon->SetPosition(screenPos);
			
			mapMoneyJarIcons_.push_back(std::move(icon));
		}
	}
}

void GameSceneUI::InitializeMapObjectIcons(const std::vector<Vector3>& chestPositions,
                                           const std::vector<Vector3>& expJarPositions, 
                                           const std::vector<Vector3>& moneyJarPositions) {
	// Chestアイコンの初期化
	mapChestIcons_.clear();
	for (const auto& pos : chestPositions) {
		auto icon = std::make_unique<Sprite>();
		icon->Initialize(chestIconPath_);
		icon->SetAnchorPoint(AnchorPoint::Center);
		
		Vector2 screenPos = WorldToMapScreen(pos);
		icon->SetPosition(screenPos);
		
		mapChestIcons_.push_back(std::move(icon));
	}
	
	// ExpJarアイコンの初期化
	mapExpJarIcons_.clear();
	for (const auto& pos : expJarPositions) {
		auto icon = std::make_unique<Sprite>();
		icon->Initialize(expJarIconPath_);
		icon->SetAnchorPoint(AnchorPoint::Center);
		
		Vector2 screenPos = WorldToMapScreen(pos);
		icon->SetPosition(screenPos);
		
		mapExpJarIcons_.push_back(std::move(icon));
	}
	
	// MoneyJarアイコンの初期化
	mapMoneyJarIcons_.clear();
	for (const auto& pos : moneyJarPositions) {
		auto icon = std::make_unique<Sprite>();
		icon->Initialize(moneyJarIconPath_);
		icon->SetAnchorPoint(AnchorPoint::Center);
		
		Vector2 screenPos = WorldToMapScreen(pos);
		icon->SetPosition(screenPos);
		
		mapMoneyJarIcons_.push_back(std::move(icon));
	}
}

void GameSceneUI::Update() {
	moneyFont_->SetNumber(moneyValue_);
	money_->Update();
	expGauge_->Update(currentExpValue_, maxExpValue_);
	hpGauge_->Update(currentHpValue_, maxHpValue_);
	currentHpFont_->SetNumber(static_cast<int>(currentHpValue_));
	maxHpFont_->SetNumber(static_cast<int>(maxHpValue_));
	hpSlash_->Update();
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

	resultTimer_.Update();
	startTimer_.Update();
	startText_->SetPosition(MyEasing::Lerp(startTextMin_, startTextMax_, startTimer_.GetProgress(), EaseType::EaseOutInSine));
	startText_->Update();

	pauseBg_->Update();
	back_->Update();
	restart_->Update();
	goTitle_->Update();

	if (resultTimer_.IsActive()) {
		resultBg_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultRestart_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultQuit_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultEnemyIcon_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultWeaponIcon1_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultWeaponIcon2_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultWeaponIcon3_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultWeaponIcon4_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultKillEnemyFont_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultKillEnemyWeaponFont1_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultKillEnemyWeaponFont2_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultKillEnemyWeaponFont3_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
		resultKillEnemyWeaponFont4_->SetColor({ 1.0f,1.0f,1.0f,resultTimer_.GetProgress() });
	}

	if (resultType_ == ResultType::GoTitle) {
		resultQuit_->SetScale(resultMax_);
		resultRestart_->SetScale(resultMin_);
	} else {
		resultQuit_->SetScale(resultMin_);
		resultRestart_->SetScale(resultMax_);
	}

	if (MyInput::Trigger(Action::R)) {
		if (isTextDraw_) {
			isTextDraw_ = false;
		} else {
			isTextDraw_ = true;
		}
	}

	resultBg_         ->Update();
	resultRestart_    ->Update();
	resultQuit_       ->Update();
	resultEnemyIcon_  ->Update();
	resultWeaponIcon1_->Update();
	resultWeaponIcon2_->Update();
	resultWeaponIcon3_->Update();
	resultWeaponIcon4_->Update();
	text_->Update();
	text2_->Update();

	lastTimer_.Update();
	if (lastTimer_.IsActive()) {
		last_->SetPosition(MyEasing::Lerp(Vector2{ -500.0f,360.0f }, { 1780.0f,360.0f }, lastTimer_.GetProgress(), EaseType::EaseOutInSine));
	}
	last_->Update();

	minMap_->Update();
	mapPlayer_->Update();
	
	if (MyInput::UseGamePad()) {
		textPad_->SetTexture("UI/game/TextPad.png");
	} else {
		textPad_->SetTexture("UI/game/TextMouse.png");
	}
	textPad_->Update();

	// Chest/Jarアイコンの更新
	for (auto& icon : mapChestIcons_) {
		icon->Update();
	}
	for (auto& icon : mapExpJarIcons_) {
		icon->Update();
	}
	for (auto& icon : mapMoneyJarIcons_) {
		icon->Update();
	}
}

void GameSceneUI::Draw() {
	expGauge_->Draw();

	hpGauge_->Draw();
	currentHpFont_->Draw();
	maxHpFont_->Draw();
	hpSlash_->Draw();

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

	if (isResultDraw_) {
		resultBg_->Draw();
		resultRestart_->Draw();
		resultQuit_->Draw();
		resultEnemyIcon_->Draw();
		resultWeaponIcon1_->Draw();
		resultWeaponIcon2_->Draw();
		resultWeaponIcon3_->Draw();
		resultWeaponIcon4_->Draw();
		resultKillEnemyFont_->Draw();
		
		// 装備している武器のみキルカウントを描画
		if (resultWeaponEquipped_[0]) {
			resultKillEnemyWeaponFont1_->Draw();
		}
		if (resultWeaponEquipped_[1]) {
			resultKillEnemyWeaponFont2_->Draw();
		}
		if (resultWeaponEquipped_[2]) {
			resultKillEnemyWeaponFont3_->Draw();
		}
		if (resultWeaponEquipped_[3]) {
			resultKillEnemyWeaponFont4_->Draw();
		}
	}
	
	if (isTextDraw_) {
		//text_->Draw();
		//text2_->Draw();
		textPad_->Draw();
	}

	// ミニマップの描画
	if (MyInput::Push(Action::MAP_OPEN)) {
		minMap_->Draw();

		// ChestとJarのアイコンを描画
		for (auto& icon : mapChestIcons_) {
			icon->Draw();
		}
		for (auto& icon : mapExpJarIcons_) {
			icon->Draw();
		}
		for (auto& icon : mapMoneyJarIcons_) {
			icon->Draw();
		}

		mapPlayer_->Draw();
	}

	startText_->Draw();
	last_->Draw();
}

void GameSceneUI::DrawImGui() {
	minMap_->DrawImGui("minMap");
	textPad_->DrawImGui("textPad");
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
	//hpSlash_->DrawImGui("HpSlash");
	//weaponIcon1_->DrawImGui("WeaponIcon1");
	//weaponIcon2_->DrawImGui("WeaponIcon2"];
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
	//resultBg_->DrawImGui("ResultBg");
	//resultRestart_->DrawImGui("ResultRestart");
	//resultQuit_->DrawImGui("ResultQuit");
	//resultEnemyIcon_->DrawImGui("ResultEnemyIcon");
	//resultWeaponIcon1_->DrawImGui("ResultWeaponIcon1");
	//resultWeaponIcon2_->DrawImGui("ResultWeaponIcon2");
	//resultWeaponIcon3_->DrawImGui("ResultWeaponIcon3");
	//resultWeaponIcon4_->DrawImGui("ResultWeaponIcon4");
	//resultKillEnemyFont_->DrawImGui("ResultKillEnemyFont");
	//resultKillEnemyWeaponFont1_->DrawImGui("ResultKillEnemyWeaponFont1");
	//resultKillEnemyWeaponFont2_->DrawImGui("ResultKillEnemyWeaponFont2");
	//resultKillEnemyWeaponFont3_->DrawImGui("ResultKillEnemyWeaponFont3");
	//resultKillEnemyWeaponFont4_->DrawImGui("ResultKillEnemyWeaponFont4");
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