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

	// JarManagerを初期化（マップ構築後に呼ぶ）
	jarManager_->Initialize(map3D_.get());

	// ChestManagerを初期化（JarManagerの後に呼んで位置情報を取得）
	chestManager_->Initialize(map3D_.get(), jarManager_.get());

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

	minJar_->Initialize("jar/minjar.obj");
	maxJar_->Initialize("jar/maxjar.obj");
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

	// JarManagerの更新
	jarManager_->Update();

	// ChestManagerの更新
	chestManager_->Update();

	//camera_ = debugCamera_;
	camera_ = gameCamera_->GetCamera();

	camera_.Update();

	map3D_->Update();

	MyDebugLine::AddGrid(100.0f, 20);
}

void GameScene::Draw() {
	MyDebugLine::Draw(camera_);

	// マップの描画
	if (map3D_) {
		map3D_->Draw(camera_);
	}

	// 壺の描画
	jarManager_->Draw(camera_);

	// 宝箱の描画
	chestManager_->Draw(camera_);

	minJar_->Draw(camera_, minJarTransform_);
	maxJar_->Draw(camera_, maxJarTransform_);

	enemyManager_->Draw(camera_);

	player_->Draw(camera_);

	collisionManager_->Draw(camera_);
}

void GameScene::DrawImGui() {
#ifdef USE_IMGUI
	auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
	postEffect->SetProjectionMatrix(camera_.GetProjectionMatrix());
	postEffect->DrawImGui();
#endif // USE_IMGUI

	//gameCamera_->DrawImgui();

	player_->DrawImGui();
	//enemyManager_->DrawImGui();

	//testParticle_->DrawImGui("TestParticle");

	// マップのImGui描画
	if (map3D_) {
		map3D_->DrawImGui();
	}
	
	// JarManagerのImGui描画
	//jarManager_->DrawImGui();

	// ChestManagerのImGui描画
	//chestManager_->DrawImGui();
}

void GameScene::PostFrameCleanup() {
	player_->PostFrameCleanup();
}