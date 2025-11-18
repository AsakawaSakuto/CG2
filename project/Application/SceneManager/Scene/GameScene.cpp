#include "GameScene.h"

GameScene::~GameScene() {
	CleanupResources();
}

void GameScene::CleanupResources() {

}

void GameScene::SetAppContext(AppContext* ctx) { ctx_ = ctx; }

void GameScene::Initialize() {
	ChangeScene(SCENE::GAME);

	gameCamera_->Initialize(ctx_);
	player_->Initialize(ctx_);

	enemyManager_->Initialize(ctx_);

	// プレイヤーにEnemyManagerへの参照を設定
	player_->SetEnemyManager(enemyManager_.get());

	// CollisionManagerを初期化し、PlayerとEnemyManagerとWeaponManagerへの参照を設定
	collisionManager_->Initialize(ctx_);
	collisionManager_->SetPlayer(player_.get());
	collisionManager_->SetEnemyManager(enemyManager_.get());
	collisionManager_->SetWeaponManager(player_->GetWeaponManager());

	testPlane_->Initialize(&ctx_->dxCommon, "plane.obj");
	testPlaneTransform_.scale = { 100.0f,1.0f,100.0f };
	testPlaneTransform_.translate = { 0.0f,-0.5f,0.0f };
	testPlane_->UseLight(false);

	testWall_->Initialize(&ctx_->dxCommon, "wall.obj");
	testWall_->SetTranslate({ 0.0f,-40.0f,0.0f });
	testWallTransform_.translate = { 0.0f,-40.0f,0.0f };

	testParticle_->Initialize(&ctx_->dxCommon);
}

void GameScene::Update() {
	if (ctx_->gamePad.TriggerButton(GamePad::START)) {
		ChangeScene(SCENE::TITLE);
	}

	player_->Update();

	gameCamera_->SetTarget(player_->GetPosition());
	gameCamera_->Update();

	enemyManager_->SetTargetPosition(player_->GetPosition());
	enemyManager_->Update();
	
	// CollisionManagerで衝突判定を実行
	collisionManager_->Update();

	camera_ = gameCamera_->GetCamera();
	camera_.Update();

	testPlane_->Update();
	testWall_->Update();

	testParticle_->Update();

	if (collisionManager_->GetGoResult()) {
		ChangeScene(SCENE::RESULT);
	}
}

void GameScene::Draw() {
	testPlane_->Draw(camera_, testPlaneTransform_);
	testWall_->Draw(camera_, testWallTransform_);

	enemyManager_->Draw(camera_);

	player_->Draw(camera_);

	//testParticle_->Draw(camera_);

	collisionManager_->Draw(camera_);
}

void GameScene::DrawImGui() {
	
	player_->DrawImGui();

	enemyManager_->DrawImGui();

	testParticle_->DrawImGui("fire");
}