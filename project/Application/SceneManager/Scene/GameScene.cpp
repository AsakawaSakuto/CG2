#include "GameScene.h"
#include "Core/ServiceLocator/ServiceLocator.h"
#include "Utility/Random/Random.h"

GameScene::~GameScene() {
	CleanupResources();
}

void GameScene::CleanupResources() {
	// PostEffectを無効化
	auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
	if (postEffect) {
		postEffect->SetEnabled(false);
	}
}

void GameScene::Initialize() {
	ChangeScene(SCENE::GAME);

	debugCamera_.SetInput(MyInput::GetInput());

	gameCamera_->Initialize();

	// ランキングを読み込む
	rankingManager_->Load();

	// 25x20x25 のマップを作成
	map3D_ = make_unique<Map3D>(15, 10, 15);
	map3D_->Initialize();
	TempMap();

	// JarManagerを初期化（マップ構築後に呼ぶ）
	jarManager_->Initialize(map3D_.get());

	// ChestManagerを初期化（JarManagerの後に呼んで位置情報を取得）
	chestManager_->Initialize(map3D_.get(), jarManager_.get());

	// TreeManagerを初期化（マップ構築後に呼ぶ）
	treeManager_->Initialize(map3D_.get());

	player_ = make_unique<Player>();
	// TitleSceneで選択されたPlayerNameとWeaponNameで初期化
	player_->Initialize(GetSelectedPlayerName(), GetSelectedWeaponName());

	// プレイヤーの開始位置を最上面のNormalブロックからランダムに選択
	auto topPositions = map3D_->GetTopNormalBlockPositions();
	if (!topPositions.empty()) {
		// ランダムに位置を選択
		int randomIndex = MyRand::Int(0, static_cast<int>(topPositions.size()) - 1);
		Vector3 startPosition = topPositions[randomIndex];
		// プレイヤーの高さを少し上に調整（ブロック上面から少し浮かせる）
		startPosition.y += 0.5f;
		player_->SetStartPos(startPosition);
	}

	enemyManager_->Initialize();

	// プレイヤーにEnemyManagerへの参照を設定
	player_->SetEnemyManager(enemyManager_.get());
	
	// EnemyManagerにPlayerへの参照を設定
	enemyManager_->SetPlayer(player_.get());

	// プレイヤーにMap3Dへの参照を設定
	player_->SetMap(map3D_.get());
	
	// プレイヤーにTreeManagerへの参照を設定
	player_->SetTreeManager(treeManager_.get());

	// CollisionManagerを初期化し、PlayerとEnemyManagerとWeaponManagerへの参照を設定
	collisionManager_->Initialize();
	collisionManager_->SetPlayer(player_.get());
	collisionManager_->SetEnemyManager(enemyManager_.get());
	collisionManager_->SetWeaponManager(player_->GetWeaponManager());

	gameSceneUI_ = make_unique<GameSceneUI>();
	gameSceneUI_->Initialize();

	wall_ = make_unique<Model>();
	wall_->Initialize("MapBlock/wall.obj");
	wall_->SetColor3({ 0.0f,0.0f,0.0f });

	auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
	postEffect->SetEnabled(true);
	postEffect->SetProjectionMatrix(camera_.GetProjectionMatrix());
	postEffect->SetPostEffectType(PSOType::PostEffect_Fog);
	postEffect->GetParams().fog.fogStart = 75.0f;
	postEffect->GetParams().fog.fogEnd = 150.0f;
	postEffect->GetParams().fog.fogDensity = 1.0f;
	postEffect->GetParams().fog.fogColor[0] = 1.0f;
	postEffect->GetParams().fog.fogColor[1] = 1.0f;
	postEffect->GetParams().fog.fogColor[2] = 1.0f;

	playTimer_.Start(300.0f, false);

	bgmNum_ = MyRand::Int(1, 5);
	MyAudio::PlayBGM(static_cast<BGM_List>(bgmNum_), bgmVolume_);

	isPause_ = false;

	fadeBG_ = make_unique<Sprite>();
	fadeBG_->Initialize("loading.png", { 0.0f,0.0f }, { 1.0f,1.0f });

	// タイトルシーン開始時はフェードアウト（透明にする）
	fadeInTimer_ = GameTimer(0.0f, false);
	fadeOutTimer_ = GameTimer(1.0f, false);
	fadeOutTimer_.Start(1.0f, false);
}

void GameScene::Update() {
	
	if (player_->IsDie() || playTimer_.IsFinished()) {
		// ランキングを保存（1回だけ）
		if (!rankingSaved_) {
			int killCount = player_->GetKillEnemyCount();
			rankingManager_->RegisterScore(killCount);
			rankingSaved_ = true;
		}

		gameSceneUI_->ResultTimerStart();
		
		// リザルトデータを更新（最初のフレームのみ）
		if (!fadeInTimer_.IsActive() && !resultDataUpdated_) {
			// 装備している武器とキルカウントを取得
			std::vector<WeaponName> equippedWeapons;
			std::vector<int> weaponKillCounts;
			
			if (player_->GetWeaponManager()) {
				const auto& weapons = player_->GetWeaponManager()->GetWeapons();
				for (const auto& weapon : weapons) {
					if (weapon) {
						WeaponName weaponName = weapon->GetWeaponName();
						equippedWeapons.push_back(weaponName);
						
						// 各武器のキルカウントを取得
						int killCount = player_->GetWeaponManager()->GetWeaponKillCount(weaponName);
						weaponKillCounts.push_back(killCount);
					}
				}
			}
			
			// 総キルカウントを取得
			int totalKills = player_->GetWeaponManager()->GetTotalWeaponKillCount();
			
			// リザルト画面のデータを更新
			gameSceneUI_->UpdateResultData(equippedWeapons, weaponKillCounts, totalKills);
			resultDataUpdated_ = true;
		}

		if (!fadeInTimer_.IsActive()) {
			if (resultType_ == ResultType::GoTitle) {
				if (MyInput::Trigger(Action::SELECT_RIGHT)) {
					MyAudio::PlaySE(SE_List::Select);
					resultType_ = ResultType::Restart;
				}
				if (MyInput::Trigger(Action::CONFIRM)) {
					MyAudio::PlaySE(SE_List::Confirm);
					fadeInTimer_.Start(1.0f, false);
				}
			} else {
				if (MyInput::Trigger(Action::SELECT_LEFT)) {
					MyAudio::PlaySE(SE_List::Select);
					resultType_ = ResultType::GoTitle;
				}
				if (MyInput::Trigger(Action::CONFIRM)) {
					MyAudio::PlaySE(SE_List::Confirm);
					fadeInTimer_.Start(1.0f, false);
				}
			}
		}

		if (fadeInTimer_.IsFinished()) {
			if (resultType_ == ResultType::GoTitle) {
				MyAudio::StopBGM(static_cast<BGM_List>(bgmNum_));
				ChangeScene(SCENE::TITLE);
			} else {
				MyAudio::StopBGM(static_cast<BGM_List>(bgmNum_));
				ChangeScene(SCENE::RESULT);
			}
		}
	}

	if (!player_->IsDie() && !playTimer_.IsFinished()) {
		if (!isPause_) {
			if (MyInput::Trigger(Action::PAUSE)) {
				pauseType_ = PauseType::Back;
				isPause_ = true;
				MyAudio::PlaySE(SE_List::Confirm);
			}

			player_->Update();

			// ゲーム中のみ更新処理を行う、アップグレード選択中は停止
			if (!player_->IsUpgradeSelect()) {

				gameCamera_->SetTarget(player_->GetPosition());
				gameCamera_->Update();

				// カメラとプレイヤー間のブロック遮蔽をチェックしてカメラ距離を調整
				gameCamera_->CheckBlockOcclusion(map3D_.get());

				// カメラとプレイヤー間の障害物を半透明化
				gameCamera_->UpdateOccluderTransparency(treeManager_.get());

				// CollisionManagerで衝突判定を実行
				collisionManager_->Update();

				enemyManager_->SetTargetPosition(player_->GetPosition());
				enemyManager_->SetMap(map3D_.get());
				enemyManager_->Update();

				JarUpdate();

				ChestUpdate();

				// ChestManagerの更新
				chestManager_->Update();

				// TreeManagerの更新
				treeManager_->Update();

				//camera_ = debugCamera_;
				camera_ = gameCamera_->GetCamera();

				camera_.Update();

				map3D_->Update();

				//UIUpdate();

				playTimer_.Update();

			}
		} else {

			if (!fadeInTimer_.IsActive()) {

				if (MyInput::Trigger(Action::CANCEL) || MyInput::Trigger(Action::PAUSE)) {
					pauseType_ = PauseType::Back;
					isPause_ = false;
					MyAudio::PlaySE(SE_List::Confirm);
				}

				switch (pauseType_) {
				case PauseType::Back:
					if (MyInput::Trigger(Action::CONFIRM)) {
						MyAudio::PlaySE(SE_List::Confirm);
						pauseType_ = PauseType::Back;
						isPause_ = false;
					}

					if (MyInput::Trigger(Action::SELECT_DOWN)) {
						MyAudio::PlaySE(SE_List::Select);
						pauseType_ = PauseType::ReStart;
					}

					break;
				case PauseType::ReStart:
					if (MyInput::Trigger(Action::CONFIRM)) {
						MyAudio::PlaySE(SE_List::Confirm);
						fadeInTimer_.Start(1.0f, false);
					}

					if (fadeInTimer_.IsFinished()) {
						MyAudio::StopBGM(static_cast<BGM_List>(bgmNum_));
						ChangeScene(SCENE::RESULT);
					}

					if (MyInput::Trigger(Action::SELECT_DOWN)) {
						MyAudio::PlaySE(SE_List::Select);
						pauseType_ = PauseType::GoTitle;
					}

					if (MyInput::Trigger(Action::SELECT_UP)) {
						MyAudio::PlaySE(SE_List::Select);
						pauseType_ = PauseType::Back;
					}

					break;
				case PauseType::GoTitle:
					if (MyInput::Trigger(Action::CONFIRM)) {
						MyAudio::PlaySE(SE_List::Confirm);
						fadeInTimer_.Start(1.0f, false);
					}

					if (fadeInTimer_.IsFinished()) {
						MyAudio::StopBGM(static_cast<BGM_List>(bgmNum_));
						ChangeScene(SCENE::TITLE);
					}

					if (MyInput::Trigger(Action::SELECT_UP)) {
						MyAudio::PlaySE(SE_List::Select);
						pauseType_ = PauseType::ReStart;
					}
					break;
				}
			}
		}
	}

	UIUpdate();

	auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
	postEffect->SetProjectionMatrix(camera_.GetProjectionMatrix());
	if (useFog_) {
		if (MyInput::TriggerKey(DIK_RETURN)) {
			useFog_ = false;
			postEffect->SetEnabled(false);
		}
	} else {
		if (MyInput::TriggerKey(DIK_RETURN)) {
			useFog_ = true;
			postEffect->SetEnabled(true);
		}
	}

	// フェードイン（GameSceneへ遷移時、徐々に不透明に）
	if (fadeInTimer_.IsActive()) {
		fadeInTimer_.Update();
		fadeBG_->SetColor({ 1.0f, 1.0f, 1.0f, fadeInTimer_.GetProgress() });
		MyAudio::SetBGMVolume(static_cast<BGM_List>(bgmNum_), bgmVolume_ * fadeInTimer_.GetReverseProgress());
	}

	// フェードアウト（TitleScene開始時、徐々に透明に）
	if (fadeOutTimer_.IsActive() && !fadeInTimer_.IsActive()) {
		fadeOutTimer_.Update();
		fadeBG_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f - fadeOutTimer_.GetProgress() });
		MyAudio::SetBGMVolume(static_cast<BGM_List>(bgmNum_), bgmVolume_ * fadeInTimer_.GetProgress());
	}

	fadeBG_->Update();
}

void GameScene::Draw() {
	
	wall_->Draw(camera_, { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{105.0f,0.0f,105.0f} });

	// マップの描画
	if (map3D_) {
		if (!MyInput::PushKey(DIK_P)) {
			map3D_->Draw(camera_);
		}
		//map3D_->Draw(camera_);
	}

	// 壺の描画
	jarManager_->Draw(camera_);

	// 宝箱の描画
	chestManager_->Draw(camera_);

	// 木の描画
	treeManager_->Draw(camera_);

	enemyManager_->Draw(camera_);

	player_->Draw(camera_);

	collisionManager_->Draw(camera_);

	MyDebugLine::Draw(camera_);

	gameSceneUI_->Draw();

	fadeBG_->Draw();
}

void GameScene::DrawImGui() {
#ifdef USE_IMGUI
	//auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
    //postEffect->SetProjectionMatrix(camera_.GetProjectionMatrix());
	//postEffect->DrawImGui();
#endif // USE_IMGUI

	//gameCamera_->DrawImgui();

	player_->DrawImGui();
	//enemyManager_->DrawImGui();

	//testParticle_->DrawImGui("TestParticle");

	// マップのImGui描画
	if (map3D_) {
		map3D_->DrawImGui();
	}
	
	gameSceneUI_->DrawImGui();

	// JarManagerのImGui描画
	//jarManager_->DrawImGui();

	// ChestManagerのImGui描画
	//chestManager_->DrawImGui();

	// TreeManagerのImGui描画
	//treeManager_->DrawImGui();
}

void GameScene::PostFrameCleanup() {
	player_->PostFrameCleanup();
}

void GameScene::JarUpdate() {

	// プレイヤーと壺の衝突判定（ExpとMoneyを取得）
	const AABB& playerAABB = player_->GetMapCollisionAABB();
	JarType jarType;
	int reward = 0;

	if (MyInput::Trigger(Action::INTERACT)) {
		reward = jarManager_->BreakJar(playerAABB, jarType);
	}

	if (reward > 0) {
		// 壺のタイプに応じてExpまたはMoneyを追加
		if (jarType == JarType::Exp) {
			player_->AddExp(reward);
		} else if (jarType == JarType::Money) {
			player_->AddMoney(reward);
		}
		// 壺を壊したときのSEを再生
		MyAudio::PlaySE(SE_List::Jar);
	}

	// JarManagerの更新
	jarManager_->SetAABBCollision(playerAABB);
	jarManager_->Update();
}

void GameScene::ChestUpdate() {
	// プレイヤーと宝箱の衝突判定
	const AABB& playerAABB = player_->GetMapCollisionAABB();
	bool isPaidChest = false;
	int openAmount = 0;

	chestManager_->SetChestActive(playerAABB, player_->GetNowMoney() >= chestManager_->GetOpenAmount());

	if (MyInput::Trigger(Action::INTERACT)) {
		// まず宝箱との衝突をチェック
		if (chestManager_->CheckChestCollision(playerAABB, isPaidChest, openAmount)) {
			bool chestOpened = false;
			if (isPaidChest) {
				// PaidChestの場合、お金をチェックしてから開ける
				if (player_->SubtractMoney(openAmount)) {
					// お金が足りる場合は宝箱を開ける
					chestOpened = chestManager_->OpenChest(playerAABB, true);
					if (chestOpened) {
						MyAudio::PlaySE(SE_List::Chest);
					}
				} else {
					// お金が足りない場合は開けない
					// TODO: お金が足りないメッセージを表示する
				}
			} else {
				// FreeChestの場合は無条件で開ける
				chestOpened = chestManager_->OpenChest(playerAABB, false);
				if (chestOpened) {
					MyAudio::PlaySE(SE_List::Chest);
				}
			}
			
			// 宝箱が開けられた場合、Upgradeを実行
			if (chestOpened && player_->GetUpgradeManager()) {
				player_->AddExp(player_->GetExpToNextLevel());
			}
		}
	}
}

void GameScene::UIUpdate() {
	gameSceneUI_->SetNowMoney(player_->GetNowMoney());
	gameSceneUI_->SetExpGauge(static_cast<float>(player_->GetCurrentExp()), static_cast<float>(player_->GetExpToNextLevel()));
	gameSceneUI_->SetHpGauge (static_cast<float>(player_->GetCurrentHP()),  static_cast<float>(player_->GetMaxHP()));
	gameSceneUI_->SetNowLv(player_->GetLevel());
	gameSceneUI_->SetKillEnemyCount(player_->GetKillEnemyCount());
	gameSceneUI_->SetChestCost(chestManager_->GetOpenAmount());
	gameSceneUI_->SetPauseType(pauseType_);
	gameSceneUI_->SetIsPaused(isPause_);
	gameSceneUI_->SetResultType(resultType_);

	// 武器アイコンの更新
	if (player_->GetWeaponManager()) {
		const auto& weapons = player_->GetWeaponManager()->GetWeapons();
		for (size_t i = 0; i < weapons.size() && i < 4; ++i) {
			if (weapons[i]) {
				WeaponName weaponName = weapons[i]->GetWeaponName();
				gameSceneUI_->UpdateWeaponIcon(static_cast<int>(i), weaponName);
				// 武器が装備されているかどうかを設定
				gameSceneUI_->SetWeaponEquipped(static_cast<int>(i), weaponName != WeaponName::None);
			}
		}
	}
	
	gameSceneUI_->SetPlayTime(playTimer_.GetRemainingTime());

	for (int i = 0; i < 4; i++) {
		gameSceneUI_->SetWeaponLv(
			i, player_->GetWeaponManager()->GetWeapons().at(i)->GetUpgradeCount());
	}

	gameSceneUI_->Update();
}

void GameScene::TempMap() {
#pragma region x 0 - 1

	map3D_->SetTile(0, 6 - 3, 0, TileType::Normal);
	map3D_->SetTile(1, 7 - 3, 0, TileType::Slope_PlusX);
	map3D_->SetTile(0, 8 - 3, 1, TileType::Normal);
	map3D_->SetTile(1, 8 - 3, 1, TileType::Normal);
	map3D_->SetTile(0, 8 - 3, 2, TileType::Slope_MinusZ);
	map3D_->SetTile(1, 8 - 3, 2, TileType::Normal);
	map3D_->SetTile(0, 7 - 3, 3, TileType::Slope_MinusZ);
	map3D_->SetTile(1, 4 - 3, 3, TileType::Normal);
	map3D_->SetTile(0, 6 - 3, 4, TileType::Slope_MinusZ);
	map3D_->SetTile(1, 5 - 3, 4, TileType::Slope_PlusZ);
	map3D_->SetTile(0, 5 - 3, 5, TileType::Normal);
	map3D_->SetTile(1, 5 - 3, 5, TileType::Normal);
	map3D_->SetTile(0, 5 - 3, 6, TileType::Normal);
	map3D_->SetTile(1, 6 - 3, 6, TileType::Slope_PlusX);
	map3D_->SetTile(0, 5 - 3, 7, TileType::Normal);
	map3D_->SetTile(1, 5 - 3, 7, TileType::Normal);
	map3D_->SetTile(0, 5 - 3, 8, TileType::Normal);
	map3D_->SetTile(1, 7 - 3, 8, TileType::Normal);
	map3D_->SetTile(0, 7 - 3, 9, TileType::Normal);
	map3D_->SetTile(1, 7 - 3, 9, TileType::Normal);
	map3D_->SetTile(0, 7 - 3, 10, TileType::Normal);
	map3D_->SetTile(1, 7 - 3, 10, TileType::Normal);
	map3D_->SetTile(0, 7 - 3, 11, TileType::Normal);
	map3D_->SetTile(1, 7 - 3, 11, TileType::Normal);
	map3D_->SetTile(0, 7 - 3, 12, TileType::Normal);
	map3D_->SetTile(1, 7 - 3, 12, TileType::Slope_MinusX);
	map3D_->SetTile(0, 7 - 3, 13, TileType::Normal);
	map3D_->SetTile(1, 7 - 3, 13, TileType::Normal);
	map3D_->SetTile(0, 7 - 3, 14, TileType::Normal);
	map3D_->SetTile(1, 7 - 3, 14, TileType::Normal);

#pragma endregion

#pragma region x 2 - 3 

	map3D_->SetTile(2, 7 - 3, 0, TileType::Normal);
	map3D_->SetTile(3, 7 - 3, 0, TileType::Slope_MinusX);
	map3D_->SetTile(2, 8 - 3, 1, TileType::Slope_PlusZ);
	map3D_->SetTile(3, 6 - 3, 1, TileType::Normal);
	map3D_->SetTile(2, 8 - 3, 2, TileType::Normal);
	map3D_->SetTile(3, 8 - 3, 2, TileType::Slope_MinusX);
	map3D_->SetTile(2, 5 - 3, 3, TileType::Slope_PlusX);
	map3D_->SetTile(3, 6 - 3, 3, TileType::Slope_PlusX);
	map3D_->SetTile(2, 5 - 3, 4, TileType::Normal);
	map3D_->SetTile(3, 4 - 3, 4, TileType::Normal);
	map3D_->SetTile(2, 5 - 3, 5, TileType::Normal);
	map3D_->SetTile(3, 4 - 3, 5, TileType::Normal);
	map3D_->SetTile(2, 6 - 3, 6, TileType::Normal);
	map3D_->SetTile(3, 5 - 3, 6, TileType::Slope_PlusZ);
	map3D_->SetTile(2, 6 - 3, 7, TileType::Normal);
	map3D_->SetTile(3, 6 - 3, 7, TileType::Slope_PlusZ);
	map3D_->SetTile(2, 7 - 3, 8, TileType::Slope_PlusZ);
	map3D_->SetTile(3, 6 - 3, 8, TileType::Normal);
	map3D_->SetTile(2, 7 - 3, 9, TileType::Normal);
	map3D_->SetTile(3, 6 - 3, 9, TileType::Normal);
	map3D_->SetTile(2, 5 - 3, 10, TileType::Normal);
	map3D_->SetTile(3, 5 - 3, 10, TileType::Slope_MinusX);
	map3D_->SetTile(2, 5 - 3, 11, TileType::Normal);
	map3D_->SetTile(3, 5 - 3, 11, TileType::Normal);
	map3D_->SetTile(2, 6 - 3, 12, TileType::Slope_MinusX);
	map3D_->SetTile(3, 5 - 3, 12, TileType::Normal);
	map3D_->SetTile(2, 6 - 3, 13, TileType::Normal);
	map3D_->SetTile(3, 5 - 3, 13, TileType::Normal);
	map3D_->SetTile(2, 7 - 3, 14, TileType::Normal);
	map3D_->SetTile(3, 7 - 3, 14, TileType::Slope_MinusX);

#pragma endregion

#pragma region x 4 - 5

	map3D_->SetTile(4, 6 - 3, 0, TileType::Normal);
	map3D_->SetTile(5, 6 - 3, 0, TileType::Normal);
	map3D_->SetTile(4, 6 - 3, 1, TileType::Normal);
	map3D_->SetTile(5, 6 - 3, 1, TileType::Normal);
	map3D_->SetTile(4, 7 - 3, 2, TileType::Slope_MinusX);
	map3D_->SetTile(5, 6 - 3, 2, TileType::Slope_MinusX);
	map3D_->SetTile(4, 6 - 3, 3, TileType::Normal);
	map3D_->SetTile(5, 6 - 3, 3, TileType::Normal);
	map3D_->SetTile(4, 4 - 3, 4, TileType::Normal);
	map3D_->SetTile(5, 6 - 3, 4, TileType::Normal);
	map3D_->SetTile(4, 4 - 3, 5, TileType::Slope_MinusZ);
	map3D_->SetTile(5, 6 - 3, 5, TileType::Normal);
	map3D_->SetTile(4, 3 - 3, 6, TileType::Normal);
	map3D_->SetTile(5, 4 - 3, 6, TileType::Slope_PlusX);
	map3D_->SetTile(4, 6 - 3, 7, TileType::Normal);
	map3D_->SetTile(5, 6 - 3, 7, TileType::Normal);
	map3D_->SetTile(4, 6 - 3, 8, TileType::Normal);
	map3D_->SetTile(5, 6 - 3, 8, TileType::Normal);
	map3D_->SetTile(4, 6 - 3, 9, TileType::Normal);
	map3D_->SetTile(5, 6 - 3, 9, TileType::Normal);
	map3D_->SetTile(4, 4 - 3, 10, TileType::Normal);
	map3D_->SetTile(5, 4 - 3, 10, TileType::Normal);
	map3D_->SetTile(4, 5 - 3, 11, TileType::Normal);
	map3D_->SetTile(5, 5 - 3, 11, TileType::Normal);
	map3D_->SetTile(4, 5 - 3, 12, TileType::Normal);
	map3D_->SetTile(5, 5 - 3, 12, TileType::Normal);
	map3D_->SetTile(4, 6 - 3, 13, TileType::Slope_PlusZ);
	map3D_->SetTile(5, 6 - 3, 13, TileType::Slope_PlusZ);
	map3D_->SetTile(4, 6 - 3, 14, TileType::Normal);
	map3D_->SetTile(5, 6 - 3, 14, TileType::Normal);

#pragma endregion

#pragma region x 6 - 7

	map3D_->SetTile(6, 6 - 3, 0, TileType::Slope_MinusX);
	map3D_->SetTile(7, 5 - 3, 0, TileType::Normal);
	map3D_->SetTile(6, 5 - 3, 1, TileType::Normal);
	map3D_->SetTile(7, 5 - 3, 1, TileType::Normal);
	map3D_->SetTile(6, 5 - 3, 2, TileType::Normal);
	map3D_->SetTile(7, 5 - 3, 2, TileType::Normal);
	map3D_->SetTile(6, 6 - 3, 3, TileType::Slope_PlusZ);
	map3D_->SetTile(7, 5 - 3, 3, TileType::Normal);
	map3D_->SetTile(6, 6 - 3, 4, TileType::Normal);
	map3D_->SetTile(7, 6 - 3, 4, TileType::Slope_PlusZ);
	map3D_->SetTile(6, 6 - 3, 5, TileType::Normal);
	map3D_->SetTile(7, 6 - 3, 5, TileType::Normal);
	map3D_->SetTile(6, 5 - 3, 6, TileType::Slope_PlusX);
	map3D_->SetTile(7, 6 - 3, 6, TileType::Slope_PlusX);
	map3D_->SetTile(6, 5 - 3, 7, TileType::Normal);
	map3D_->SetTile(7, 5 - 3, 7, TileType::Slope_MinusX);
	map3D_->SetTile(6, 5 - 3, 8, TileType::Normal);
	map3D_->SetTile(7, 5 - 3, 8, TileType::Slope_MinusX);
	map3D_->SetTile(6, 6 - 3, 9, TileType::Slope_MinusX);
	map3D_->SetTile(7, 5 - 3, 9, TileType::Slope_MinusX);
	map3D_->SetTile(6, 4 - 3, 10, TileType::Normal);
	map3D_->SetTile(7, 4 - 3, 10, TileType::Normal);
	map3D_->SetTile(6, 4 - 3, 11, TileType::Normal);
	map3D_->SetTile(7, 4 - 3, 11, TileType::Normal);
	map3D_->SetTile(6, 5 - 3, 12, TileType::Normal);
	map3D_->SetTile(7, 7 - 3, 12, TileType::Normal);
	map3D_->SetTile(6, 6 - 3, 13, TileType::Slope_PlusZ);
	map3D_->SetTile(7, 7 - 3, 13, TileType::Normal);
	map3D_->SetTile(6, 6 - 3, 14, TileType::Normal);
	map3D_->SetTile(7, 6 - 3, 14, TileType::Slope_MinusX);

#pragma endregion

#pragma region  x 8 - 9

	map3D_->SetTile(8, 6 - 3, 0, TileType::Slope_PlusX);
	map3D_->SetTile(9, 6 - 3, 0, TileType::Normal);
	map3D_->SetTile(8, 6 - 3, 1, TileType::Slope_PlusX);
	map3D_->SetTile(9, 6 - 3, 1, TileType::Normal);
	map3D_->SetTile(8, 6 - 3, 2, TileType::Slope_PlusX);
	map3D_->SetTile(9, 6 - 3, 2, TileType::Normal);
	map3D_->SetTile(8, 5 - 3, 3, TileType::Normal);
	map3D_->SetTile(9, 6 - 3, 3, TileType::Slope_MinusZ);
	map3D_->SetTile(8, 5 - 3, 4, TileType::Normal);
	map3D_->SetTile(9, 5 - 3, 4, TileType::Normal);
	map3D_->SetTile(8, 6 - 3, 5, TileType::Slope_PlusZ);
	map3D_->SetTile(9, 5 - 3, 5, TileType::Normal);
	map3D_->SetTile(8, 6 - 3, 6, TileType::Normal);
	map3D_->SetTile(9, 5 - 3, 6, TileType::Normal);
	map3D_->SetTile(8, 4 - 3, 7, TileType::Normal);
	map3D_->SetTile(9, 4 - 3, 7, TileType::Normal);
	map3D_->SetTile(8, 4 - 3, 8, TileType::Normal);
	map3D_->SetTile(9, 4 - 3, 8, TileType::Normal);
	map3D_->SetTile(8, 4 - 3, 9, TileType::Normal);
	map3D_->SetTile(9, 4 - 3, 9, TileType::Normal);
	map3D_->SetTile(8, 4 - 3, 10, TileType::Normal);
	map3D_->SetTile(9, 4 - 3, 10, TileType::Normal);
	map3D_->SetTile(8, 4 - 3, 11, TileType::Normal);
	map3D_->SetTile(9, 4 - 3, 11, TileType::Slope_MinusZ);
	map3D_->SetTile(8, 7 - 3, 12, TileType::Normal);
	map3D_->SetTile(9, 3 - 3, 12, TileType::Normal);
	map3D_->SetTile(8, 7 - 3, 13, TileType::Normal);
	map3D_->SetTile(9, 7 - 3, 13, TileType::Slope_MinusX);
	map3D_->SetTile(8, 5 - 3, 14, TileType::Normal);
	map3D_->SetTile(9, 5 - 3, 14, TileType::Normal);

#pragma endregion

#pragma region x 10 - 11

	map3D_->SetTile(10, 6 - 3, 0, TileType::Normal);
	map3D_->SetTile(11, 6 - 3, 0, TileType::Normal);
	map3D_->SetTile(10, 6 - 3, 1, TileType::Normal);
	map3D_->SetTile(11, 6 - 3, 1, TileType::Normal);
	map3D_->SetTile(10, 5 - 3, 2, TileType::Normal);
	map3D_->SetTile(11, 5 - 3, 2, TileType::Normal);
	map3D_->SetTile(10, 6 - 3, 3, TileType::Slope_PlusZ);
	map3D_->SetTile(11, 5 - 3, 3, TileType::Normal);
	map3D_->SetTile(10, 6 - 3, 4, TileType::Normal);
	map3D_->SetTile(11, 6 - 3, 4, TileType::Normal);
	map3D_->SetTile(10, 6 - 3, 5, TileType::Slope_PlusX);
	map3D_->SetTile(11, 6 - 3, 5, TileType::Normal);
	map3D_->SetTile(10, 5 - 3, 6, TileType::Normal);
	map3D_->SetTile(11, 5 - 3, 6, TileType::Normal);
	map3D_->SetTile(10, 5 - 3, 7, TileType::Normal);
	map3D_->SetTile(11, 5 - 3, 7, TileType::Normal);
	map3D_->SetTile(10, 5 - 3, 8, TileType::Slope_PlusX);
	map3D_->SetTile(11, 5 - 3, 8, TileType::Normal);
	map3D_->SetTile(10, 5 - 3, 9, TileType::Slope_PlusX);
	map3D_->SetTile(11, 5 - 3, 9, TileType::Normal);
	map3D_->SetTile(10, 3 - 3, 10, TileType::Normal);
	map3D_->SetTile(11, 4 - 3, 10, TileType::Slope_PlusX);
	map3D_->SetTile(10, 3 - 3, 11, TileType::Normal);
	map3D_->SetTile(11, 5 - 3, 11, TileType::Normal);
	map3D_->SetTile(10, 3 - 3, 12, TileType::Normal);
	map3D_->SetTile(11, 5 - 3, 12, TileType::Normal);
	map3D_->SetTile(10, 6 - 3, 13, TileType::Slope_MinusX);
	map3D_->SetTile(11, 5 - 3, 13, TileType::Normal);
	map3D_->SetTile(10, 5 - 3, 14, TileType::Normal);
	map3D_->SetTile(11, 5 - 3, 14, TileType::Normal);

#pragma endregion

#pragma region x 12 - 13

	map3D_->SetTile(12, 6 - 3, 0, TileType::Slope_MinusX);
	map3D_->SetTile(13, 5 - 3, 0, TileType::Slope_MinusX);
	map3D_->SetTile(12, 6 - 3, 1, TileType::Normal);
	map3D_->SetTile(13, 3 - 3, 1, TileType::Normal);
	map3D_->SetTile(12, 6 - 3, 2, TileType::Normal);
	map3D_->SetTile(13, 6 - 3, 2, TileType::Slope_MinusX);
	map3D_->SetTile(12, 6 - 3, 3, TileType::Normal);
	map3D_->SetTile(13, 6 - 3, 3, TileType::Slope_MinusX);
	map3D_->SetTile(12, 5 - 3, 4, TileType::Normal);
	map3D_->SetTile(13, 5 - 3, 4, TileType::Normal);
	map3D_->SetTile(12, 5 - 3, 5, TileType::Normal);
	map3D_->SetTile(13, 6 - 3, 5, TileType::Slope_PlusX);
	map3D_->SetTile(12, 5 - 3, 6, TileType::Normal);
	map3D_->SetTile(13, 6 - 3, 6, TileType::Slope_PlusX);
	map3D_->SetTile(12, 5 - 3, 7, TileType::Normal);
	map3D_->SetTile(13, 5 - 3, 7, TileType::Normal);
	map3D_->SetTile(12, 5 - 3, 8, TileType::Normal);
	map3D_->SetTile(13, 5 - 3, 8, TileType::Normal);
	map3D_->SetTile(12, 5 - 3, 9, TileType::Normal);
	map3D_->SetTile(13, 5 - 3, 9, TileType::Normal);
	map3D_->SetTile(12, 5 - 3, 10, TileType::Normal);
	map3D_->SetTile(13, 5 - 3, 10, TileType::Normal);
	map3D_->SetTile(12, 5 - 3, 10, TileType::Slope_PlusX);
	map3D_->SetTile(13, 6 - 3, 10, TileType::Slope_PlusX);
	map3D_->SetTile(12, 6 - 3, 11, TileType::Slope_PlusX);
	map3D_->SetTile(13, 6 - 3, 11, TileType::Normal);
	map3D_->SetTile(12, 5 - 3, 12, TileType::Normal);
	map3D_->SetTile(13, 6 - 3, 12, TileType::Normal);
	map3D_->SetTile(12, 6 - 3, 13, TileType::Slope_PlusX);
	map3D_->SetTile(13, 6 - 3, 13, TileType::Normal);
	map3D_->SetTile(12, 5 - 3, 14, TileType::Normal);
	map3D_->SetTile(13, 5 - 3, 14, TileType::Normal);

#pragma endregion

#pragma region x 14 - 15

	map3D_->SetTile(14, 4 - 3, 0, TileType::Slope_MinusX);
	map3D_->SetTile(14, 3 - 3, 1, TileType::Normal);
	map3D_->SetTile(14, 5 - 3, 2, TileType::Normal);
	map3D_->SetTile(14, 5 - 3, 3, TileType::Normal);
	map3D_->SetTile(14, 6 - 3, 4, TileType::Slope_PlusZ);
	map3D_->SetTile(14, 6 - 3, 5, TileType::Normal);
	map3D_->SetTile(14, 6 - 3, 6, TileType::Normal);
	map3D_->SetTile(14, 6 - 3, 7, TileType::Slope_MinusZ);
	map3D_->SetTile(14, 5 - 3, 8, TileType::Normal);
	map3D_->SetTile(14, 6 - 3, 9, TileType::Slope_PlusZ);
	map3D_->SetTile(14, 6 - 3, 10, TileType::Normal);
	map3D_->SetTile(14, 6 - 3, 11, TileType::Normal);
	map3D_->SetTile(14, 6 - 3, 12, TileType::Normal);
	map3D_->SetTile(14, 6 - 3, 13, TileType::Slope_MinusZ);
	map3D_->SetTile(14, 5 - 3, 14, TileType::Normal);

#pragma endregion
}