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
		player_->SetPosition(startPosition);
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
}

void GameScene::Update() {
	if (MyInput::Trigger(Action::PAUSE)) {
		ChangeScene(SCENE::TITLE);
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

		UIUpdate();

		playTimer_.Update();

	}

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

	if (MyInput::Trigger(Action::INTERACT)) {
		// まず宝箱との衝突をチェック
		if (chestManager_->CheckChestCollision(playerAABB, isPaidChest, openAmount)) {
			bool chestOpened = false;
			if (isPaidChest) {
				// PaidChestの場合、お金をチェックしてから開ける
				if (player_->SubtractMoney(openAmount)) {
					// お金が足りる場合は宝箱を開ける
					chestOpened = chestManager_->OpenChest(playerAABB, true);
				} else {
					// お金が足りない場合は開けない
					// TODO: お金が足りないメッセージを表示する
				}
			} else {
				// FreeChestの場合は無条件で開ける
				chestOpened = chestManager_->OpenChest(playerAABB, false);
			}
			
			// 宝箱が開けられた場合、Upgradeを実行
			if (chestOpened && player_->GetUpgradeManager()) {
				player_->GetUpgradeManager()->Upgrade();
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
	
	// 武器アイコンの更新
	if (player_->GetWeaponManager()) {
		const auto& weapons = player_->GetWeaponManager()->GetWeapons();
		for (size_t i = 0; i < weapons.size() && i < 4; ++i) {
			if (weapons[i]) {
				WeaponName weaponName = weapons[i]->GetWeaponName();
				gameSceneUI_->UpdateWeaponIcon(static_cast<int>(i), weaponName);
			}
		}
	}
	
	gameSceneUI_->SetPlayTime(playTimer_.GetRemainingTime());

	gameSceneUI_->Update();
}

void GameScene::TempMap() {
#pragma region x 0 - 1

	map3D_->SetTile(0, 6, 0, TileType::Normal);
	map3D_->SetTile(1, 7, 0, TileType::Slope_PlusX);

	map3D_->SetTile(0, 8, 1, TileType::Normal);
	map3D_->SetTile(1, 8, 1, TileType::Normal);

	map3D_->SetTile(0, 8, 2, TileType::Slope_MinusZ);
	map3D_->SetTile(1, 8, 2, TileType::Normal);

	map3D_->SetTile(0, 7, 3, TileType::Slope_MinusZ);
	map3D_->SetTile(1, 4, 3, TileType::Normal);

	map3D_->SetTile(0, 6, 4, TileType::Slope_MinusZ);
	map3D_->SetTile(1, 5, 4, TileType::Slope_PlusZ);

	map3D_->SetTile(0, 5, 5, TileType::Normal);
	map3D_->SetTile(1, 5, 5, TileType::Normal);

	map3D_->SetTile(0, 5, 6, TileType::Normal);
	map3D_->SetTile(1, 6, 6, TileType::Slope_PlusX);

	map3D_->SetTile(0, 5, 7, TileType::Normal);
	map3D_->SetTile(1, 5, 7, TileType::Normal);

	map3D_->SetTile(0, 5, 8, TileType::Normal);
	map3D_->SetTile(1, 7, 8, TileType::Normal);

	map3D_->SetTile(0, 7, 9, TileType::Normal);
	map3D_->SetTile(1, 7, 9, TileType::Normal);

	map3D_->SetTile(0, 7, 10, TileType::Normal);
	map3D_->SetTile(1, 7, 10, TileType::Normal);

	map3D_->SetTile(0, 7, 11, TileType::Normal);
	map3D_->SetTile(1, 7, 11, TileType::Normal);

	map3D_->SetTile(0, 7, 12, TileType::Normal);
	map3D_->SetTile(1, 7, 12, TileType::Slope_MinusX);

	map3D_->SetTile(0, 7, 13, TileType::Normal);
	map3D_->SetTile(1, 7, 13, TileType::Normal);

	map3D_->SetTile(0, 7, 14, TileType::Normal);
	map3D_->SetTile(1, 7, 14, TileType::Normal);

#pragma endregion

#pragma region x 2 - 3 

	map3D_->SetTile(2, 7, 0, TileType::Normal);
	map3D_->SetTile(3, 7, 0, TileType::Slope_MinusX);

	map3D_->SetTile(2, 8, 1, TileType::Slope_PlusZ);
	map3D_->SetTile(3, 6, 1, TileType::Normal);

	map3D_->SetTile(2, 8, 2, TileType::Normal);
	map3D_->SetTile(3, 8, 2, TileType::Slope_MinusX);

	map3D_->SetTile(2, 5, 3, TileType::Slope_PlusX);
	map3D_->SetTile(3, 6, 3, TileType::Slope_PlusX);

	map3D_->SetTile(2, 5, 4, TileType::Normal);
	map3D_->SetTile(3, 4, 4, TileType::Normal);

	map3D_->SetTile(2, 5, 5, TileType::Normal);
	map3D_->SetTile(3, 4, 5, TileType::Normal);

	map3D_->SetTile(2, 6, 6, TileType::Normal);
	map3D_->SetTile(3, 5, 6, TileType::Slope_PlusZ);

	map3D_->SetTile(2, 6, 7, TileType::Normal);
	map3D_->SetTile(3, 6, 7, TileType::Slope_PlusZ);

	map3D_->SetTile(2, 7, 8, TileType::Slope_PlusZ);
	map3D_->SetTile(3, 6, 8, TileType::Normal);

	map3D_->SetTile(2, 7, 9, TileType::Normal);
	map3D_->SetTile(3, 6, 9, TileType::Normal);

	map3D_->SetTile(2, 5, 10, TileType::Normal);
	map3D_->SetTile(3, 5, 10, TileType::Slope_MinusX);

	map3D_->SetTile(2, 5, 11, TileType::Normal);
	map3D_->SetTile(3, 5, 11, TileType::Normal);

	map3D_->SetTile(2, 6, 12, TileType::Slope_MinusX);
	map3D_->SetTile(3, 5, 12, TileType::Normal);

	map3D_->SetTile(2, 6, 13, TileType::Normal);
	map3D_->SetTile(3, 5, 13, TileType::Normal);

	map3D_->SetTile(2, 7, 14, TileType::Normal);
	map3D_->SetTile(3, 7, 14, TileType::Slope_MinusX);

#pragma endregion

#pragma region x 4 - 5

	map3D_->SetTile(4, 6, 0, TileType::Normal);
	map3D_->SetTile(5, 6, 0, TileType::Normal);

	map3D_->SetTile(4, 6, 1, TileType::Normal);
	map3D_->SetTile(5, 6, 1, TileType::Normal);

	map3D_->SetTile(4, 7, 2, TileType::Slope_MinusX);
	map3D_->SetTile(5, 6, 2, TileType::Slope_MinusX);

	map3D_->SetTile(4, 6, 3, TileType::Normal);
	map3D_->SetTile(5, 6, 3, TileType::Normal);

	map3D_->SetTile(4, 4, 4, TileType::Normal);
	map3D_->SetTile(5, 6, 4, TileType::Normal);

	map3D_->SetTile(4, 4, 5, TileType::Slope_MinusZ);
	map3D_->SetTile(5, 6, 5, TileType::Normal);

	map3D_->SetTile(4, 3, 6, TileType::Normal);
	map3D_->SetTile(5, 4, 6, TileType::Slope_PlusX);

	map3D_->SetTile(4, 6, 7, TileType::Normal);
	map3D_->SetTile(5, 6, 7, TileType::Normal);

	map3D_->SetTile(4, 6, 8, TileType::Normal);
	map3D_->SetTile(5, 6, 8, TileType::Normal);

	map3D_->SetTile(4, 6, 9, TileType::Normal);
	map3D_->SetTile(5, 6, 9, TileType::Normal);

	map3D_->SetTile(4, 4, 10, TileType::Normal);
	map3D_->SetTile(5, 4, 10, TileType::Normal);

	map3D_->SetTile(4, 5, 11, TileType::Normal);
	map3D_->SetTile(5, 5, 11, TileType::Normal);

	map3D_->SetTile(4, 5, 12, TileType::Normal);
	map3D_->SetTile(5, 5, 12, TileType::Normal);

	map3D_->SetTile(4, 6, 13, TileType::Slope_PlusZ);
	map3D_->SetTile(5, 6, 13, TileType::Slope_PlusZ);

	map3D_->SetTile(4, 6, 14, TileType::Normal);
	map3D_->SetTile(5, 6, 14, TileType::Normal);

#pragma endregion

#pragma region x 6 - 7

	map3D_->SetTile(6, 6, 0, TileType::Slope_MinusX);
	map3D_->SetTile(7, 5, 0, TileType::Normal);

	map3D_->SetTile(6, 5, 1, TileType::Normal);
	map3D_->SetTile(7, 5, 1, TileType::Normal);

	map3D_->SetTile(6, 5, 2, TileType::Normal);
	map3D_->SetTile(7, 5, 2, TileType::Normal);

	map3D_->SetTile(6, 6, 3, TileType::Slope_PlusZ);
	map3D_->SetTile(7, 5, 3, TileType::Normal);

	map3D_->SetTile(6, 6, 4, TileType::Normal);
	map3D_->SetTile(7, 6, 4, TileType::Slope_PlusZ);

	map3D_->SetTile(6, 6, 5, TileType::Normal);
	map3D_->SetTile(7, 6, 5, TileType::Normal);

	map3D_->SetTile(6, 5, 6, TileType::Slope_PlusX);
	map3D_->SetTile(7, 6, 6, TileType::Slope_PlusX);

	map3D_->SetTile(6, 5, 7, TileType::Normal);
	map3D_->SetTile(7, 5, 7, TileType::Slope_MinusX);

	map3D_->SetTile(6, 5, 8, TileType::Normal);
	map3D_->SetTile(7, 5, 8, TileType::Slope_MinusX);

	map3D_->SetTile(6, 6, 9, TileType::Slope_MinusX);
	map3D_->SetTile(7, 5, 9, TileType::Slope_MinusX);

	map3D_->SetTile(6, 4, 10, TileType::Normal);
	map3D_->SetTile(7, 4, 10, TileType::Normal);

	map3D_->SetTile(6, 4, 11, TileType::Normal);
	map3D_->SetTile(7, 4, 11, TileType::Normal);

	map3D_->SetTile(6, 5, 12, TileType::Normal);
	map3D_->SetTile(7, 7, 12, TileType::Normal);

	map3D_->SetTile(6, 6, 13, TileType::Slope_PlusZ);
	map3D_->SetTile(7, 7, 13, TileType::Normal);

	map3D_->SetTile(6, 6, 14, TileType::Normal);
	map3D_->SetTile(7, 6, 14, TileType::Slope_MinusX);

#pragma endregion

#pragma region  x 8 - 9

	map3D_->SetTile(8, 6, 0, TileType::Slope_PlusX);
	map3D_->SetTile(9, 6, 0, TileType::Normal);

	map3D_->SetTile(8, 6, 1, TileType::Slope_PlusX);
	map3D_->SetTile(9, 6, 1, TileType::Normal);

	map3D_->SetTile(8, 6, 2, TileType::Slope_PlusX);
	map3D_->SetTile(9, 6, 2, TileType::Normal);

	map3D_->SetTile(8, 5, 3, TileType::Normal);
	map3D_->SetTile(9, 6, 3, TileType::Slope_MinusZ);

	map3D_->SetTile(8, 5, 4, TileType::Normal);
	map3D_->SetTile(9, 5, 4, TileType::Normal);

	map3D_->SetTile(8, 6, 5, TileType::Slope_PlusZ);
	map3D_->SetTile(9, 5, 5, TileType::Normal);

	map3D_->SetTile(8, 6, 6, TileType::Normal);
	map3D_->SetTile(9, 5, 6, TileType::Normal);

	map3D_->SetTile(8, 4, 7, TileType::Normal);
	map3D_->SetTile(9, 4, 7, TileType::Normal);

	map3D_->SetTile(8, 4, 8, TileType::Normal);
	map3D_->SetTile(9, 4, 8, TileType::Normal);

	map3D_->SetTile(8, 4, 9, TileType::Normal);
	map3D_->SetTile(9, 4, 9, TileType::Normal);

	map3D_->SetTile(8, 4, 10, TileType::Normal);
	map3D_->SetTile(9, 4, 10, TileType::Normal);

	map3D_->SetTile(8, 4, 11, TileType::Normal);
	map3D_->SetTile(9, 4, 11, TileType::Slope_MinusZ);

	map3D_->SetTile(8, 7, 12, TileType::Normal);
	map3D_->SetTile(9, 3, 12, TileType::Normal);

	map3D_->SetTile(8, 7, 13, TileType::Normal);
	map3D_->SetTile(9, 7, 13, TileType::Slope_MinusX);

	map3D_->SetTile(8, 5, 14, TileType::Normal);
	map3D_->SetTile(9, 5, 14, TileType::Normal);

#pragma endregion

#pragma region x 10 - 11

	map3D_->SetTile(10, 6, 0, TileType::Normal);
	map3D_->SetTile(11, 6, 0, TileType::Normal);

	map3D_->SetTile(10, 6, 1, TileType::Normal);
	map3D_->SetTile(11, 6, 1, TileType::Normal);

	map3D_->SetTile(10, 5, 2, TileType::Normal);
	map3D_->SetTile(11, 5, 2, TileType::Normal);

	map3D_->SetTile(10, 6, 3, TileType::Slope_PlusZ);
	map3D_->SetTile(11, 5, 3, TileType::Normal);

	map3D_->SetTile(10, 6, 4, TileType::Normal);
	map3D_->SetTile(11, 6, 4, TileType::Normal);

	map3D_->SetTile(10, 6, 5, TileType::Slope_PlusX);
	map3D_->SetTile(11, 6, 5, TileType::Normal);

	map3D_->SetTile(10, 5, 6, TileType::Normal);
	map3D_->SetTile(11, 5, 6, TileType::Normal);

	map3D_->SetTile(10, 5, 7, TileType::Normal);
	map3D_->SetTile(11, 5, 7, TileType::Normal);

	map3D_->SetTile(10, 5, 8, TileType::Slope_PlusX);
	map3D_->SetTile(11, 5, 8, TileType::Normal);

	map3D_->SetTile(10, 5, 9, TileType::Slope_PlusX);
	map3D_->SetTile(11, 5, 9, TileType::Normal);

	map3D_->SetTile(10, 3, 10, TileType::Normal);
	map3D_->SetTile(11, 4, 10, TileType::Slope_PlusX);

	map3D_->SetTile(10, 3, 11, TileType::Normal);
	map3D_->SetTile(11, 5, 11, TileType::Normal);

	map3D_->SetTile(10, 3, 12, TileType::Normal);
	map3D_->SetTile(11, 5, 12, TileType::Normal);

	map3D_->SetTile(10, 6, 13, TileType::Slope_MinusX);
	map3D_->SetTile(11, 5, 13, TileType::Normal);

	map3D_->SetTile(10, 5, 14, TileType::Normal);
	map3D_->SetTile(11, 5, 14, TileType::Normal);

#pragma endregion

#pragma region x 12 - 13

	map3D_->SetTile(12, 6, 0, TileType::Slope_MinusX);
	map3D_->SetTile(13, 5, 0, TileType::Slope_MinusX);

	map3D_->SetTile(12, 6, 1, TileType::Normal);
	map3D_->SetTile(13, 3, 1, TileType::Normal);

	map3D_->SetTile(12, 6, 2, TileType::Normal);
	map3D_->SetTile(13, 6, 2, TileType::Slope_MinusX);

	map3D_->SetTile(12, 6, 3, TileType::Normal);
	map3D_->SetTile(13, 6, 3, TileType::Slope_MinusX);

	map3D_->SetTile(12, 5, 4, TileType::Normal);
	map3D_->SetTile(13, 5, 4, TileType::Normal);

	map3D_->SetTile(12, 5, 5, TileType::Normal);
	map3D_->SetTile(13, 6, 5, TileType::Slope_PlusX);

	map3D_->SetTile(12, 5, 6, TileType::Normal);
	map3D_->SetTile(13, 6, 6, TileType::Slope_PlusX);

	map3D_->SetTile(12, 5, 7, TileType::Normal);
	map3D_->SetTile(13, 5, 7, TileType::Normal);

	map3D_->SetTile(12, 5, 8, TileType::Normal);
	map3D_->SetTile(13, 5, 8, TileType::Normal);

	map3D_->SetTile(12, 5, 9, TileType::Normal);
	map3D_->SetTile(13, 5, 9, TileType::Normal);

	map3D_->SetTile(12, 5, 10, TileType::Normal);
	map3D_->SetTile(13, 5, 10, TileType::Normal);

	map3D_->SetTile(12, 5, 10, TileType::Slope_PlusX);
	map3D_->SetTile(13, 6, 10, TileType::Slope_PlusX);

	map3D_->SetTile(12, 6, 11, TileType::Slope_PlusX);
	map3D_->SetTile(13, 6, 11, TileType::Normal);

	map3D_->SetTile(12, 5, 12, TileType::Normal);
	map3D_->SetTile(13, 6, 12, TileType::Normal);

	map3D_->SetTile(12, 6, 13, TileType::Slope_PlusX);
	map3D_->SetTile(13, 6, 13, TileType::Normal);

	map3D_->SetTile(12, 5, 14, TileType::Normal);
	map3D_->SetTile(13, 5, 14, TileType::Normal);

#pragma endregion

#pragma region x 14 - 15

	map3D_->SetTile(14, 4, 0, TileType::Slope_MinusX);

	map3D_->SetTile(14, 3, 1, TileType::Normal);

	map3D_->SetTile(14, 5, 2, TileType::Normal);

	map3D_->SetTile(14, 5, 3, TileType::Normal);

	map3D_->SetTile(14, 6, 4, TileType::Slope_PlusZ);

	map3D_->SetTile(14, 6, 5, TileType::Normal);

	map3D_->SetTile(14, 6, 6, TileType::Normal);

	map3D_->SetTile(14, 6, 7, TileType::Slope_MinusZ);

	map3D_->SetTile(14, 5, 8, TileType::Normal);

	map3D_->SetTile(14, 6, 9, TileType::Slope_PlusZ);

	map3D_->SetTile(14, 6, 10, TileType::Normal);

	map3D_->SetTile(14, 6, 11, TileType::Normal);

	map3D_->SetTile(14, 6, 12, TileType::Normal);

	map3D_->SetTile(14, 6, 13, TileType::Slope_MinusZ);

	map3D_->SetTile(14, 5, 14, TileType::Normal);

#pragma endregion
}