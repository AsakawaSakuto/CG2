#include "GameScene.h"
#include "Core/ServiceLocator/ServiceLocator.h"

GameScene::~GameScene() {
	CleanupResources();
}

void GameScene::CleanupResources() {

}

void GameScene::Initialize() {
	ChangeScene(SCENE::GAME);

	debugCamera_.SetInput(MyInput::GetInput());

	gameCamera_->Initialize();

	// ===== Map3D のサンプル実装 =====
	// 25x20x25 のマップを作成
	map3D_ = make_unique<Map3D>(15, 10, 15);
	map3D_->Initialize();

	// サンプル: 床を作成（y=0の全面にNormalブロックを配置）
	for (uint32_t z = 0; z < map3D_->GetDepth(); ++z) {
		for (uint32_t x = 0; x < map3D_->GetWidth(); ++x) {
			map3D_->SetTile(x, 0, z, TileType::Normal);
		}
	}

	// map3D_->SetTile(14, 1, 0, TileType::Normal);
	// map3D_->SetTile(15, 1, 0, TileType::Normal);

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


	player_ = make_unique<Player>();
	player_->Initialize();

	enemyManager_->Initialize();

	// プレイヤーにEnemyManagerへの参照を設定
	player_->SetEnemyManager(enemyManager_.get());

	// プレイヤーにMap3Dへの参照を設定
	player_->SetMap(map3D_.get());

	// CollisionManagerを初期化し、PlayerとEnemyManagerとWeaponManagerへの参照を設定
	collisionManager_->Initialize();
	collisionManager_->SetPlayer(player_.get());
	collisionManager_->SetEnemyManager(enemyManager_.get());
	collisionManager_->SetWeaponManager(player_->GetWeaponManager());

	testParticle_->Initialize();
	dustParticle_->Initialize();
	dustParticle_->LoadJson("dust");

	lv_ = make_unique<Sprite>();
	lv_->Initialize("UI/game/lv.png", { 70.0f, 110.0f }, { 0.7f, 0.7f });

	lvText_ = make_unique<Sprite>();
	lvText_->Initialize("UI/game/lvText.png", { 140.0f, 180.0f }, { 0.35f, 0.5f } );

	fireBallIcon_ = make_unique<Sprite>();
	fireBallIcon_->Initialize("icon/FireBall.png", { 275.0f, 175.0f }, { 1.5f, 1.5f });

	leaserIcon_ = make_unique<Sprite>();
	leaserIcon_->Initialize("icon/leaser.png", { 330.0f, 175.0f }, { 1.5f, 1.5f });

	runaIcon_ = make_unique<Sprite>();
	runaIcon_->Initialize("icon/Runa.png", { 385.0f, 175.0f }, { 1.5f, 1.5f });

	text_ = make_unique<Sprite>();
	text_->Initialize("UI/game/text.png", { 0.0f, 0.0f }, { 1.0f, 1.0f });
	textMoveTimer_.Start(2.0f, false);

	// ビットマップフォントの初期化
	timeFont_ = make_unique<BitmapFont>();
	timeFont_->Initialize("number/");
	timeFont_->SetScale({ 0.6f, 0.6f });
	timeFont_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 白色

	playerHPFont_ = make_unique<BitmapFont>();
	playerHPFont_->Initialize("number/");
	playerHPFont_->SetScale({ 0.4f, 0.4f });
	playerHPFont_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 赤色

	playerLv_ = make_unique<BitmapFont>();
	playerLv_->Initialize("number/");
	playerLv_->SetScale({ 0.4f, 0.4f });

	// HPゲージの初期化
	hpGauge_ = make_unique<Gauge>();
	hpGauge_->Initialize();
	hpGauge_->SetGaugeColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // 緑色
	hpGauge_->SetBackgroundColor({ 0.3f, 0.0f, 0.0f, 1.0f }); // 暗い赤

	// 経験値ゲージの初期化
	expGauge_ = make_unique<Gauge>();
	expGauge_->Initialize();
	expGauge_->SetGaugeColor({ 0.0f, 0.8f, 1.0f, 1.0f }); // シアン色
	expGauge_->SetBackgroundColor({ 0.1f, 0.1f, 0.3f, 1.0f }); // 暗い青

	gameTime_ = 0.0f;
	score_ = 0;

	wallModel_->Initialize("mapBlock/wall.obj");
}

void GameScene::Update() {
	if (MyInput::Trigger(Action::PAUSE)) {
		ChangeScene(SCENE::TITLE);
	}

	player_->Update();

	gameCamera_->SetTarget(player_->GetPosition());
	gameCamera_->Update();

	// CollisionManagerで衝突判定を実行
	collisionManager_->Update();

	enemyManager_->SetTargetPosition(player_->GetPosition());
	enemyManager_->Update();

	camera_ = gameCamera_->GetCamera();
	//debugCamera_.Update();
	//camera_ = debugCamera_;
	camera_.Update();

	testParticle_->Update();
	dustParticle_->Update();

	MyDebugLine::AddGrid(100.0f, 20);

	if (textMoveTimer_.IsActive()) {
		text_->SetPosition({ MyEasing::Lerp(1780.0f, -500.0f, textMoveTimer_.GetProgress(),EaseType::EaseOutInSine), 360.0f });
		textMoveTimer_.Update();
	}

	text_->Update();
	lv_->Update();
	lvText_->Update();
	fireBallIcon_->Update();
	leaserIcon_->Update();
	runaIcon_->Update();

	// ゲーム時間を更新
	gameTime_ += 1.0f / 60.0f; // 仮に60FPSとして計算

	// スコアを更新（例：テスト用に毎フレーム1増加）
	// score_++;

	// ビットマップフォントの更新

	/*timeFont_->SetTime(gameTime_, { 1100.0f, 50.0f }, 60.0f);
	timeFont_->Update();

	playerHPFont_->SetNumber(player_->GetCurrentHP(), { 215.0f,50.0f }, 60.0f);
	playerHPFont_->Update();

	playerLv_->SetNumber(player_->GetLevel(), {122.0f,112.0f}, 55.0f);
	playerLv_->SetScale({0.5f,0.5f});
	playerLv_->Update();*/

	// ゲージの更新
	hpGauge_->Update(player_->GetCurrentHP(), player_->GetMaxHP());
	expGauge_->Update(player_->GetCurrentExp(), player_->GetExpToNextLevel());

	map3D_->Update();
}

void GameScene::Draw() {
	MyDebugLine::Draw(camera_);

	//wallModel_->Draw(camera_);

	// マップの描画
	if (map3D_) {
		map3D_->Draw(camera_);
	}

	enemyManager_->Draw(camera_);

	player_->Draw(camera_);

	testParticle_->Draw(camera_);
	dustParticle_->Draw(camera_);

	collisionManager_->Draw(camera_);

	text_->Draw();
	lv_->Draw();
	lvText_->Draw();
	fireBallIcon_->Draw();
	leaserIcon_->Draw();
	runaIcon_->Draw();

	//MyParticle::DrawAll(camera_);

	// ゲージの描画
	hpGauge_->Draw();
	expGauge_->Draw();

	// ビットマップフォントの描画
	timeFont_->Draw();
	playerHPFont_->Draw();
	playerLv_->Draw();
}

void GameScene::DrawImGui() {
#ifdef USE_IMGUI
	auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
	postEffect->SetProjectionMatrix(camera_.GetProjectionMatrix());
	postEffect->DrawImGui();
#endif // USE_IMGUI

	player_->DrawImGui();
	//enemyManager_->DrawImGui();

	//testParticle_->DrawImGui("TestParticle");

	// マップのImGui描画
	if (map3D_) {
		map3D_->DrawImGui();
	}
}

void GameScene::PostFrameCleanup() {
	player_->PostFrameCleanup();
}